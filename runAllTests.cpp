/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#define _CXXTEST_HAVE_STD
#define _CXXTEST_HAVE_EH
#define CXXTEST_TRAP_SIGNALS
#define _CXXTEST_ABORT_TEST_ON_FAIL
#define CXXTEST_STACK_TRACE_EXE "Debug/GeneNet"

#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/XmlStdioPrinter.h>

typedef const CxxTest::SuiteDescription *SuiteDescriptionPtr;
typedef const CxxTest::TestDescription *TestDescriptionPtr;

#include "myTestSuite.h"
static MyTestSuite suite_MyTestSuite;

static CxxTest::List Tests_MyTestSuite;
CxxTest::StaticSuiteDescription suiteDescription_MyTestSuite;

static class TestDescription_MyTestSuite_testThresholds : public CxxTest::RealTestDescription {
public:
 void runTest() { suite_MyTestSuite.testThresholds(); }
} testDescription_MyTestSuite_testThresholds;

static class TestDescription_MyTestSuite_testSpecie : public CxxTest::RealTestDescription {
public:
 void runTest() { suite_MyTestSuite.testSpecie(); }
} testDescription_MyTestSuite_testSpecie;

static class TestDescription_MyTestSuite_testSpecies : public CxxTest::RealTestDescription {
public:
 void runTest() { suite_MyTestSuite.testSpecies(); }
} testDescription_MyTestSuite_testSpecies;

static class TestDescription_MyTestSuite_testExperiments : public CxxTest::RealTestDescription {
public:
 void runTest() { suite_MyTestSuite.testExperiments(); }
} testDescription_MyTestSuite_testExperiments;

static class TestDescription_MyTestSuite_testSet : public CxxTest::RealTestDescription {
public:
 void runTest() { suite_MyTestSuite.testSet(); }
} testDescription_MyTestSuite_testSet;

static class TestDescription_MyTestSuite_testDoubleSet : public CxxTest::RealTestDescription {
public:
 void runTest() { suite_MyTestSuite.testDoubleSet(); }
} testDescription_MyTestSuite_testDoubleSet;

static class TestDescription_MyTestSuite_testEncodings : public CxxTest::RealTestDescription {
public:
 void runTest() { suite_MyTestSuite.testEncodings(); }
} testDescription_MyTestSuite_testEncodings;

static class TestDescription_MyTestSuite_testNetCon : public CxxTest::RealTestDescription {
public:
 void runTest() { suite_MyTestSuite.testNetCon(); }
} testDescription_MyTestSuite_testNetCon;

static class TestDescription_MyTestSuite_testLevelAssignments : public CxxTest::RealTestDescription {
public:
 void runTest() { suite_MyTestSuite.testLevelAssignments(); }
} testDescription_MyTestSuite_testLevelAssignments;

static class TestDescription_MyTestSuite_testincrementBaseSet : public CxxTest::RealTestDescription {
public:
 void runTest() { suite_MyTestSuite.testincrementBaseSet(); }
} testDescription_MyTestSuite_testincrementBaseSet;

static class TestDescription_MyTestSuite_testGeneNet : public CxxTest::RealTestDescription {
public:
 void runTest() { suite_MyTestSuite.testGeneNet(); }
} testDescription_MyTestSuite_testGeneNet;

namespace CxxTest {
 void initialize()
 {
  Tests_MyTestSuite.initialize();
  suiteDescription_MyTestSuite.initialize( "myTestSuite.h", 20, "MyTestSuite", suite_MyTestSuite, Tests_MyTestSuite );
  testDescription_MyTestSuite_testThresholds.initialize( Tests_MyTestSuite, suiteDescription_MyTestSuite, 28, "testThresholds" );
  testDescription_MyTestSuite_testSpecie.initialize( Tests_MyTestSuite, suiteDescription_MyTestSuite, 36, "testSpecie" );
  testDescription_MyTestSuite_testSpecies.initialize( Tests_MyTestSuite, suiteDescription_MyTestSuite, 62, "testSpecies" );
  testDescription_MyTestSuite_testExperiments.initialize( Tests_MyTestSuite, suiteDescription_MyTestSuite, 88, "testExperiments" );
  testDescription_MyTestSuite_testSet.initialize( Tests_MyTestSuite, suiteDescription_MyTestSuite, 200, "testSet" );
  testDescription_MyTestSuite_testDoubleSet.initialize( Tests_MyTestSuite, suiteDescription_MyTestSuite, 344, "testDoubleSet" );
  testDescription_MyTestSuite_testEncodings.initialize( Tests_MyTestSuite, suiteDescription_MyTestSuite, 508, "testEncodings" );
  testDescription_MyTestSuite_testNetCon.initialize( Tests_MyTestSuite, suiteDescription_MyTestSuite, 649, "testNetCon" );
  testDescription_MyTestSuite_testLevelAssignments.initialize( Tests_MyTestSuite, suiteDescription_MyTestSuite, 717, "testLevelAssignments" );
  testDescription_MyTestSuite_testincrementBaseSet.initialize( Tests_MyTestSuite, suiteDescription_MyTestSuite, 875, "testincrementBaseSet" );
  testDescription_MyTestSuite_testGeneNet.initialize( Tests_MyTestSuite, suiteDescription_MyTestSuite, 922, "testGeneNet" );
 }
}
#include <cxxtest/Root.cpp>
#define MW_STACK_TRACE_INITIAL_PREFIX CXXTEST_STACK_TRACE_INITIAL_PREFIX
#define MW_STACK_TRACE_OTHER_PREFIX CXXTEST_STACK_TRACE_INITIAL_PREFIX
#define MW_XML_OUTPUT_FILE ".memwatchResults"

#include <cxxtest/Memwatch.cpp>
#include <signal.h>    // for siginfo_t and signal constants
#include <setjmp.h>	   // for siglongjmp()
#include <stdlib.h>	   // for exit()

void __cxxtest_sig_handler( int, siginfo_t*, void* ) _CXXTEST_NO_INSTR;

void __cxxtest_sig_handler( int signum, siginfo_t* /*info*/, void* /*arg*/ )
{
    const char* msg = "run-time exception";
    switch ( signum )
    {
        case SIGFPE:
	    msg = "SIGFPE: floating point exception (div by zero?)";
	    // Currently, can't get cygwin g++ to pass in info,
            // so we can't be more specific.
	    break;
        case SIGSEGV:
            msg = "SIGSEGV: segmentation fault (null pointer dereference?)";
            break;
        case SIGILL:
            msg = "SIGILL: illegal instruction "
		"(dereference uninitialized or deleted pointer?)";
            break;
        case SIGTRAP:
            msg = "SIGTRAP: trace trap";
            break;
#ifdef SIGEMT
        case SIGEMT:
            msg = "SIGEMT: EMT instruction";
            break;
#endif
        case SIGBUS:
            msg = "SIGBUS: bus error "
		"(dereference uninitialized or deleted pointer?)";
            break;
        case SIGSYS:
            msg = "SIGSYS: bad argument to system call";
            break;
        case SIGABRT:
            msg = "SIGABRT: execution aborted "
		"(failed assertion, corrupted heap, or other problem?)";
            break;
    }
    if ( !CxxTest::__cxxtest_assertmsg.empty() )
    {
	CxxTest::__cxxtest_sigmsg = CxxTest::__cxxtest_assertmsg;
	CxxTest::__cxxtest_assertmsg = "";
    }
    else if ( CxxTest::__cxxtest_sigmsg.empty() )
    {
	CxxTest::__cxxtest_sigmsg = msg;
    }
    else
    {
	CxxTest::__cxxtest_sigmsg = std::string(msg)
	    + ", maybe related to " + CxxTest::__cxxtest_sigmsg;
    }
    
#ifdef CXXTEST_TRACE_STACK
    {
        std::string trace = CxxTest::getStackTrace();
        if ( trace.length() )
        {
            CxxTest::__cxxtest_sigmsg += "\n";
            CxxTest::__cxxtest_sigmsg += trace;
        }
    }
#endif
    if ( CxxTest::__cxxtest_jmppos >= 0 )
    {
	siglongjmp( CxxTest::__cxxtest_jmpbuf[CxxTest::__cxxtest_jmppos], 1 );
    }
    else
    {
        std::cout << "\nError: untrapped signal:\n"
	    << CxxTest::__cxxtest_sigmsg
            << "\n"; // std::endl;
	exit(1);
    }
}
class CxxTestMain {
public:
    CxxTestMain() {
    struct sigaction act;
    // act.sa_handler = __cxxtest_sig_handler;
    // act.sa_flags = 0;
    act.sa_sigaction = __cxxtest_sig_handler;
    act.sa_flags = SA_SIGINFO;
    sigaction( SIGSEGV, &act, 0 );
    sigaction( SIGFPE,  &act, 0 );
    sigaction( SIGILL,  &act, 0 );
    sigaction( SIGBUS,  &act, 0 );
    sigaction( SIGABRT, &act, 0 );
    sigaction( SIGTRAP, &act, 0 );
#ifdef SIGEMT
    sigaction( SIGEMT,  &act, 0 );
#endif
    sigaction( SIGSYS,  &act, 0 );

        CxxTest::initialize();
 FILE* resultsFile = fopen(".cxxtestResults", "w");
 CxxTest::XmlStdioPrinter(resultsFile).run();
 fclose(resultsFile);
    }
};
CxxTestMain cxxTestMain __attribute__((init_priority(65535)));;

