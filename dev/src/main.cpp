// Copyright 2014 Wouter van Oortmerssen. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "stdafx.h"

#include "compiler.h"
#include "vm.h"

// FIXME: This makes SDL not modular, but without it it will miss the SDLMain indirection.
#include "sdlincludes.h"
#include "sdlinterface.h"

using namespace lobster;

int main(int argc, char* argv[]) {
    #ifdef _WIN32
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    #endif
    Output(OUTPUT_INFO, "Lobster running...");
    InitTime();
    bool wait = false;
    bool from_bundle =
    #ifdef __IOS__
        true;
    #else
        false;
    #endif
    try {
        RegisterCoreEngineBuiltins();
        bool parsedump = false;
        bool disasm = false;
        bool to_cpp = false;
        const char *default_bcf = "default.lbc";
        const char *bcf = nullptr;
        const char *fn = nullptr;
        string helptext = "\nUsage:\n"
            "lobster [ OPTIONS ] [ FILE ]\n"
            "Compile & run FILE, or omit FILE to load default.lbc\n"
            "-w                     Wait for input before exiting.\n"
            "-b                     Compile to bytecode file, don't run.\n"
            "--to-cpp               Compile to C++ code, don't run.\n"
            "--parsedump            Also dump parse tree.\n"
            "--disasm               Also dump bytecode disassembly.\n"
            "--verbose              Output additional informational text.\n"
            "--debug                Output compiler internal logging.\n"
            "--silent               Only output errors.\n"
            "--gen-builtins-html    Write builtin commands help file.\n"
            "--gen-builtins-names   Write builtin commands - just names.\n"
            "--non-interactive-test Quit after running 1 frame.\n";
        for (int arg = 1; arg < argc; arg++) {
            if (argv[arg][0] == '-') {
                string a = argv[arg];
                if (a == "-w") { wait = true; }
                else if (a == "-b") { bcf = default_bcf; }
                else if (a == "--to-cpp") { to_cpp = true; }
                else if (a == "--parsedump") { parsedump = true; }
                else if (a == "--disasm") { disasm = true; }
                else if (a == "--verbose") { min_output_level = OUTPUT_INFO; }
                else if (a == "--debug") { min_output_level = OUTPUT_DEBUG; }
                else if (a == "--silent") { min_output_level = OUTPUT_ERROR; }
                else if (a == "--gen-builtins-html") { DumpBuiltins(false); return 0; }
                else if (a == "--gen-builtins-names") { DumpBuiltins(true); return 0; }
                else if (a == "--non-interactive-test") { SDLTestMode(); }
                // process identifier supplied by OS X
                else if (a.substr(0, 5) == "-psn_") { from_bundle = true; }
                else throw "unknown command line argument: " + (argv[arg] + helptext);
            } else {
                if (fn) throw "more than one file specified" + helptext;
                fn = argv[arg];
            }
        }
        #ifdef __IOS__
            //fn = "totslike.lobster";  // FIXME: temp solution
        #endif
        if (!SetupDefaultDirs(argv[0], fn, from_bundle))
            throw string("cannot find location to read/write data on this platform!");
        vector<uchar> bytecode;
        if (!fn) {
            if (!LoadByteCode(default_bcf, bytecode))
                throw "Lobster programming language compiler/runtime (version " __DATE__
                      ")\nno arguments given - cannot load " + (default_bcf + helptext);
        } else {
            Output(OUTPUT_INFO, "compiling...");
            string dump;
            Compile(StripDirPart(fn).c_str(), nullptr, bytecode, parsedump ? &dump : nullptr);
            if (parsedump) {
                FILE *f = OpenForWriting("parsedump.txt", false);
                if (f) {
                    fprintf(f, "%s\n", dump.c_str());
                    fclose(f);
                }
            }
            if (bcf) {
                SaveByteCode(bcf, bytecode);
                return 0;
            }
        }
        if (disasm) {
            FILE *f = OpenForWriting("disasm.txt", false);
            if (f) {
                string s;
                DisAsm(s, bytecode.data());
                fputs(s.c_str(), f);
                fclose(f);
            }
        }
        if (to_cpp) {
            // FIXME: make less hard-coded.
            FILE *f = fopen((StripFilePart(argv[0]) +
                            "../dev/compiled_lobster/src/compiled_lobster.cpp").c_str(), "w");
            if (f) {
                string s;
                ToCPP(s, bytecode.data(), bytecode.size());
                fputs(s.c_str(), f);
                fclose(f);
            }
        } else {
            if (EngineRunByteCode(fn, std::move(bytecode), nullptr, nullptr))
                return 0;  // Emscripten inverted control.
        }
    }
    catch (string &s) {
        Output(OUTPUT_ERROR, s.c_str());
        if (from_bundle) MsgBox(s.c_str());
        if (wait) {
            Output(OUTPUT_PROGRAM, "press <ENTER> to continue:\n");
            getchar();
        }
        #ifdef _WIN32
            _CrtSetDbgFlag(0);  // Don't bother with memory leaks when there was an error.
        #endif
        EngineExit(1);
    }
    EngineExit(0);
    return 0;
}

