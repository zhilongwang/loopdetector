/*
 * Copyright 2002-2019 Intel Corporation.
 * 
 * This software is provided to you as Sample Source Code as defined in the accompanying
 * End User License Agreement for the Intel(R) Software Development Products ("Agreement")
 * section 1.L.
 * 
 * This software and the related documents are provided as is, with no express or implied
 * warranties, other than those that are expressly stated in the License.
 */

#include <iostream>
#include <fstream>
#include "pin.H"
#include "../include/loop.h"
using std::cerr;
using std::ofstream;
using std::ios;
using std::string;
using std::endl;
using std::cout;
using std::hex;
ofstream OutFile;
Loop* loop = nullptr;
// The running count of instructions is kept here
// make it static to help the compiler optimize docount
static UINT64 icount = 0;

// This function is called before every block
VOID processBB(ADDRINT bbhead, UINT32 insnum, ADDRINT sp, ADDRINT nexthead) { 
    cout << "[BB]@" << hex << bbhead << " -> " << nexthead << "\t[ins]:" << insnum << "\t[sp]:" << sp << endl;
    loop->process_bb(bbhead, insnum, sp, nexthead);
}

VOID processRET(ADDRINT sp){
    loop->clear_stack(sp);
}
    
// Pin calls this function every time a new basic block is encountered
// It inserts a call to docount
VOID Trace(TRACE trace, VOID *v)
{
    // Visit every basic block  in the trace
    for (BBL bbl = TRACE_BblHead(trace); BBL_Valid(bbl); bbl = BBL_Next(bbl))
    {
        // Insert a call to docount before every bbl, passing the number of instructions
        ADDRINT next_bbhead = 0;
        BBL next_bb = BBL_Next(bbl);
        if ( BBL_INVALID(next_bb)){
            next_bbhead = BBL_Address(next_bb);
        }
        INS tail_ins = BBL_InsTail(bbl);
        xed_iclass_enum_t ins_indx = (xed_iclass_enum_t)INS_Opcode(tail_ins);
        switch (ins_indx) {
            case XED_ICLASS_RET_FAR:
            case XED_ICLASS_RET_NEAR:
                BBL_InsertCall(bbl, IPOINT_BEFORE, (AFUNPTR)processBB, IARG_ADDRINT, BBL_Address(bbl), IARG_UINT32, BBL_NumIns(bbl), IARG_REG_VALUE, REG_ESP, IARG_ADDRINT, 0,IARG_END);
                INS_InsertCall(tail_ins, IPOINT_BEFORE, AFUNPTR(processRET), IARG_REG_VALUE, REG_ESP, IARG_END);
                break;
            case XED_ICLASS_CALL_NEAR:
            case XED_ICLASS_CALL_FAR:
                BBL_InsertCall(bbl, IPOINT_BEFORE, (AFUNPTR)processBB, IARG_ADDRINT, BBL_Address(bbl), IARG_UINT32, BBL_NumIns(bbl), IARG_REG_VALUE, REG_ESP, IARG_ADDRINT, BBL_Address(next_bb),IARG_END);
                break;
            default:
                BBL_InsertCall(bbl, IPOINT_BEFORE, (AFUNPTR)processBB, IARG_ADDRINT, BBL_Address(bbl), IARG_UINT32, BBL_NumIns(bbl), IARG_REG_VALUE, REG_ESP, IARG_ADDRINT, 0,IARG_END);
                break;
        }
    }
}

KNOB<string> KnobOutputFile(KNOB_MODE_WRITEONCE, "pintool",
    "o", "inscount.out", "specify output file name");

// This function is called when the application exits
VOID Fini(INT32 code, VOID *v)
{
    // Write to a file since cout and cerr maybe closed by the application
    OutFile.setf(ios::showbase);
    OutFile << "Count " << icount << endl;
    OutFile.close();
}
// This function is called when the thread starts
VOID ThreadStart(THREADID threadid, CONTEXT *ctxt, INT32 flags, VOID *v) {
    loop = new Loop();
}
// This function is called when the thread exits
VOID ThreadFini(THREADID threadIndex, const CONTEXT *ctxt, INT32 code, VOID *v) {
    loop->fini();
}

/* ===================================================================== */
/* Print Help Message                                                    */
/* ===================================================================== */

INT32 Usage()
{
    cerr << "This tool counts the number of dynamic instructions executed" << endl;
    cerr << endl << KNOB_BASE::StringKnobSummary() << endl;
    return -1;
}

/* ===================================================================== */
/* Main                                                                  */
/* ===================================================================== */

int main(int argc, char * argv[])
{
    // Initialize pin
    if (PIN_Init(argc, argv)) return Usage();

    OutFile.open(KnobOutputFile.Value().c_str());

    // Register ThreadStart to be called when a thread starts.
    PIN_AddThreadStartFunction(ThreadStart, NULL);

    // Register Fini to be called when the application exits.
    PIN_AddFiniFunction(Fini, NULL);

    // Register Instruction to be called to instrument instructions
    TRACE_AddInstrumentFunction(Trace, 0);

    // Register Fini to be called when the application exits
    PIN_AddFiniFunction(Fini, 0);
    
    // Start the program, never returns
    PIN_StartProgram();
    
    return 0;
}
