 /******************************************************************************
 * INCLUDE FILES
 ******************************************************************************/
#include <svc.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#define AUTOTEST
#ifdef AUTOTEST
#include "TestLib.h"
#endif

/*******************************************************************************
 * DEFINES
 ******************************************************************************/
#define TEST_CHECK(x)   if (x) {nPass++;} else {nFail++; TLIB_LOG(msg);} 
#define LOG_DETAIL          0
#define WAIT_TIME           0
#define LOOPS               100
#define FILE_NONMODIFIED    1
#define FILE_MODIFIED       0
#define TESTCASES           9

/*******************************************************************************
 * FUNCTIONS DECLARATIONS
 ******************************************************************************/
void vTest_MultiFileOpenWriteClose(void);
void vTest_MultiFileWrite(void);                              
void vTest_MultiCreateFile(void); 
void vTest_OpenAttributes(void);
void vTest_OpenAttributesNeg(void);            
void vTest_SeekSetTest(void); 
void vTest_SeekCurFromSetTest(void);
void vTest_SeekCurFromEndTest(void);
void vTest_SeekEndTest(void);
void vInit(void);
void vWaitKeyPress(void);
 

/*******************************************************************************
 * CLASSES
 ******************************************************************************/ 
class CSDFile
{
        char sFileName[50];
        char sDataRead[100];
        int nHandle;
    public:
        int nRecordLen;
        CSDFile(const char *s);
        ~CSDFile();
        void vOpen(const int attrib, const int exp_ret, const int exp_err);
        void vLSeek(const long offset, const int origin, const int exp_ret, const int exp_err);
        void vWrite(const int line, const int exp_ret, const int exp_err);
        void vRead(const int line, const int exp_ret, const int exp_err);
        void vClose();
        void vCreateFile();
        void vWriteToFile(const int line);
        void vReadCheckFile(const int linecount, const int mode);
        void vModify();
};

/*******************************************************************************
 * GLOBAL VARIABLES
 ******************************************************************************/ 
int nFail = 0;
int nPass = 0;
char msg[200] = "";
char extra_str[100] = "1234567890ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char drive[4] = "";

/*******************************************************************************
 * METHODS
 ******************************************************************************/ 
CSDFile::CSDFile(const char *s)
{
    sprintf(sFileName, "%c:%s", drive[0], s);
    nRecordLen = strlen(sFileName) + strlen(extra_str) + 12;
} 
 
CSDFile::~CSDFile()
{
} 

void CSDFile::vOpen(const int attrib, const int exp_ret, const int exp_err)
{
    int errnum  = 0;
    
    errno = 0;
    nHandle = open(sFileName, attrib);      
    errnum = errno;
    memset(msg, 0, sizeof(msg));
    sprintf(msg, "open %s returns %d with errno %d", sFileName, nHandle, errnum);
    if ( LOG_DETAIL )
    {    
        TLIB_LOG(msg);
        if ( exp_ret >= 0 )
        {    
            TLIB_ASSERT( (nHandle > exp_ret) && (errnum == exp_err) );
        }
        else
        {
            TLIB_ASSERT( (nHandle == exp_ret) && (errnum == exp_err) );
        }
    }
    else
    {
        if ( exp_ret >= 0 )
        {    
            TEST_CHECK( (nHandle > exp_ret) && (errnum == exp_err) );
        }
        else
        {
            TEST_CHECK( (nHandle == exp_ret) && (errnum == exp_err) );
        }
    }
}    

void CSDFile::vLSeek(const long offset, const int origin, const int exp_ret, const int exp_err)
{
    int errnum  = 0;
    int ret     = 0;
    
    errno = 0;
    ret = lseek(nHandle, offset, origin);      
    errnum = errno;
    memset(msg, 0, sizeof(msg));
    sprintf(msg, "lseek(%d, %d) %s returns %d with errno %d", offset, origin, sFileName, ret, errnum);
    if ( LOG_DETAIL )
    {
        TLIB_LOG(msg);
        if ( exp_ret == 0 )
        {    
            TLIB_ASSERT( (ret >= exp_ret) && (errnum == exp_err) );
        }
        else
        {
            TLIB_ASSERT( (ret == exp_ret) && (errnum == exp_err) );
        }       
    }
    else
    {
        if ( exp_ret == 0 )
        {    
            TEST_CHECK( (ret >= exp_ret) && (errnum == exp_err) );
        }
        else
        {
            TEST_CHECK( (ret == exp_ret) && (errnum == exp_err) );
        }      
    }
}

void CSDFile::vWrite(const int line, const int exp_ret, const int exp_err)
{
    int errnum  = 0;
    int ret     = 0;
    char s[100]  = "";
    
    gotoxy(1, 3);
    printf("WRITE %s LINE%.5d     \n", sFileName, line);
    
    sprintf(s, "%s LINE%.5d %s\n", sFileName, line, extra_str);
    errno = 0;
    ret = write(nHandle, s, strlen(s));      
    errnum = errno;
    memset(msg, 0, sizeof(msg));
    sprintf(msg, "write %s to %s returns %d with errno %d", s, sFileName, ret, errnum);
    if ( LOG_DETAIL )
    {            
        TLIB_LOG(msg);
        if ( exp_ret >= 0 )
        {    
            TLIB_ASSERT( (ret == strlen(s)) && (errnum == exp_err) );
        }
        else
        {
            TLIB_ASSERT( (ret == exp_ret) && (errnum == exp_err) );
        }
    }
    else
    {
        if ( exp_ret >= 0 )
        {    
            TEST_CHECK( (ret == strlen(s)) && (errnum == exp_err) );
        }
        else
        {
            TEST_CHECK( (ret == exp_ret) && (errnum == exp_err) );
        }
    }
}

void CSDFile::vRead(const int line, const int exp_ret, const int exp_err)
{
    int errnum  = 0;
    int ret     = 0;
    char buffer[100] = "";
    char exp_str[100] = "";
    
    gotoxy(1, 3);
    clreol();
    printf("READ %s\n", sFileName);
    
    memset(exp_str, 0, sizeof(exp_str));
    sprintf(exp_str, "%s LINE%.5d %s\n", sFileName, line, extra_str);
    memset(buffer, 0, sizeof(buffer));
    errno = 0;
    ret = read(nHandle, buffer, strlen(exp_str));
    errnum = errno;
    memset(msg, 0, sizeof(msg));
    sprintf(msg, "read %s returns %d with errno %d", sFileName, ret, errnum);
    if ( LOG_DETAIL )
    {                
        TLIB_LOG(msg);
        if ( exp_ret == 0 )
        {    
            TLIB_ASSERT( (ret == strlen(exp_str)) && (errnum == exp_err) );
        }
        else
        {
            TLIB_ASSERT( (ret == exp_ret) && (errnum == exp_err) );
        }
    }
    else
    {
        if ( exp_ret == 0 )
        {    
            TEST_CHECK( (ret == strlen(exp_str)) && (errnum == exp_err) );
        }
        else
        {
            TEST_CHECK( (ret == exp_ret) && (errnum == exp_err) );
        }
    }    

    memset(msg, 0, sizeof(msg));
    if ( buffer == NULL )
    {
        sprintf(msg, "read data = NULL");
    }
    else if ( exp_str == NULL )
    {
        sprintf(msg, "exp data = NULL");
    }
    else
    {
        sprintf(msg, "read data = %s, exp data = %s", buffer, exp_str);    
    }
    if ( LOG_DETAIL )
    {                
        TLIB_LOG(msg);
        TLIB_ASSERT( strcmp(exp_str, buffer) == 0 );
    }
    else
    {
        TEST_CHECK( strcmp(exp_str, buffer) == 0 );
    }
}    

void CSDFile::vClose()
{
    int errnum  = 0;
    int ret     = 0;
    
    errno = 0;
    ret = close(nHandle);      
    errnum = errno;
    memset(msg, 0, sizeof(msg));
    sprintf(msg, "close %s returns %d with errno %d", sFileName, ret, errnum);
    if ( LOG_DETAIL )
    {                
        TLIB_LOG(msg);
        TLIB_ASSERT( (ret == 0) && (errnum == 0) );
    }
    else
    {
        TEST_CHECK( (ret == 0) && (errnum == 0) );
    }   
    
}    

void CSDFile::vCreateFile()
{
    gotoxy(1, 3);
    printf("CREATE %s\n", sFileName);
    
    vOpen(O_CREAT | O_TRUNC, 0, 0);      
    vClose();
}

void CSDFile::vWriteToFile(const int line)
{
    vOpen(O_RDWR, 0, 0);
    vLSeek(0, SEEK_END, 0, 0);
    vWrite(line, 0, 0);
    vClose();
    SVC_WAIT(WAIT_TIME);
}

void CSDFile::vReadCheckFile(const int linecount, const int mode)
{
    int errnum  = 0;
    int ret     = 0;
    int i       = 0;
    char buffer[100] = "";
    char exp_str[100] = "";
    
    gotoxy(1, 1);
    clreol();
    printf("READ %s\n", sFileName);
    
    vOpen(O_RDONLY, 0, 0);
    vLSeek(0, SEEK_SET, 0, 0);
    for ( i = 1; i <= linecount; i++ )
    {
        memset(exp_str, 0, sizeof(exp_str));
        if ((mode == FILE_MODIFIED) && (( i == 1 ) || ( i == 2 ) || ( i == linecount )))
        {
            sprintf(exp_str, "%s LINE00000 %s\n", sFileName, extra_str);
        }
        else
        {
            sprintf(exp_str, "%s LINE%.5d %s\n", sFileName, i, extra_str);
        }
        
        memset(buffer, 0, sizeof(buffer));
        errno = 0;
        ret = read(nHandle, buffer, strlen(exp_str));
        errnum = errno;
        memset(msg, 0, sizeof(msg));
        sprintf(msg, "read %s returns %d with errno %d", sFileName, ret, errnum);
        if ( LOG_DETAIL )
        {                
            TLIB_LOG(msg);
            TLIB_ASSERT( (ret == strlen(exp_str)) && (errnum == 0) );
        }
        else
        {
            TEST_CHECK( (ret == strlen(exp_str)) && (errnum == 0) );
        }    
            
        
        memset(msg, 0, sizeof(msg));
        if ( buffer == NULL )
        {
            sprintf(msg, "read data = NULL");
        }
        else if ( exp_str == NULL )
        {
            sprintf(msg, "exp data = NULL");
        }
        else
        {
            sprintf(msg, "read data = %s, exp data = %s", buffer, exp_str);    
        }
        if ( LOG_DETAIL )
        {                
            TLIB_LOG(msg);
            TLIB_ASSERT( strcmp(exp_str, buffer) == 0 );
        }
        else
        {
            TEST_CHECK( strcmp(exp_str, buffer) == 0 );
        }
    }
    vClose();
}

//Test modification of 1st, middle and last record
void CSDFile::vModify()
{
    char exp_str[100]  = "";
    
    gotoxy(1, 1);
    clreol();
    printf("MODIFY %s\n", sFileName);
    
    //modify first record
    vOpen(O_RDWR, 0, 0);
    vLSeek(0, SEEK_SET, 0, 0);
    vWrite(0, 0, 0);
    
    //modify second record
    vLSeek(0, SEEK_CUR, 0, 0);
    vWrite(0, 0, 0);
    
    //modify last record
    memset(exp_str, 0, sizeof(exp_str));
    sprintf(exp_str, "%s LINE%.5d %s\n", sFileName, LOOPS, extra_str); 
    vLSeek(-(strlen(exp_str)), SEEK_END, 0, 0);
    vWrite(0, 0, 0);
    
    vClose();
}

/*******************************************************************************
 * MAIN FUNCTION
 ******************************************************************************/
int main()
{
    TLIB_AGENT();
    vInit();
    TLIB_LOG("%s@%d: test_status.",__FILE__,__LINE__);vTest_MultiFileOpenWriteClose();
    TLIB_LOG("%s@%d: test_status.",__FILE__,__LINE__);vTest_MultiFileWrite();
    TLIB_LOG("%s@%d: test_status.",__FILE__,__LINE__);vTest_MultiCreateFile();
    TLIB_LOG("%s@%d: test_status.",__FILE__,__LINE__);vTest_OpenAttributes();
    TLIB_LOG("%s@%d: test_status.",__FILE__,__LINE__);vTest_OpenAttributesNeg();
    TLIB_LOG("%s@%d: test_status.",__FILE__,__LINE__);vTest_SeekSetTest();
    TLIB_LOG("%s@%d: test_status.",__FILE__,__LINE__);vTest_SeekCurFromSetTest();
    TLIB_LOG("%s@%d: test_status.",__FILE__,__LINE__);vTest_SeekCurFromEndTest();
    TLIB_LOG("%s@%d: test_status.",__FILE__,__LINE__);vTest_SeekEndTest();
    clrscr();
    printf("TEST END");    
    
    TLIB_APP_END();
    return 0;
}

/*******************************************************************************
 * FUNCTION DEFINITIONS
 ******************************************************************************/ 
void vInit(void)
{
    get_env("DRIVE", drive, 1);
}

void vTest_MultiFileOpenWriteClose(void)
{
    int i = 0;
    CSDFile sdFile1("FILE1.DAT");
    CSDFile sdFile2("FILE2.DAT");
    CSDFile sdFile3("FILE3.DAT");
    CSDFile sdFile4("FILE4.DAT");
    CSDFile sdFile5("FILE5.DAT");
    
    TLIB_SUITE("Testing Multiple File Open Write");
    clrscr();
    nFail = 0;
    nPass = 0;
    
    //create files
    sdFile1.vCreateFile();
    sdFile2.vCreateFile();
    sdFile3.vCreateFile();
    sdFile4.vCreateFile();
    sdFile5.vCreateFile();
    
    //write to files
    for ( i = 1; i <= LOOPS; i++)
    {
        gotoxy(1, 1);
        clreol();
        printf("TEST 1/%d\n", TESTCASES);
        printf("PROGRESS: %d%%  \n", ((i * 100)/LOOPS));
            
        sdFile1.vWriteToFile(i);
        sdFile2.vWriteToFile(i);
        sdFile3.vWriteToFile(i);
        sdFile4.vWriteToFile(i);
        sdFile5.vWriteToFile(i);
    }    
    
    //modify
    sdFile1.vModify();
    sdFile2.vModify();
    sdFile3.vModify();
    sdFile4.vModify();
    sdFile5.vModify();
    
    //check files
    clrscr(); 
    sdFile1.vReadCheckFile(LOOPS, FILE_MODIFIED);
    sdFile2.vReadCheckFile(LOOPS, FILE_MODIFIED);
    sdFile3.vReadCheckFile(LOOPS, FILE_MODIFIED);
    sdFile4.vReadCheckFile(LOOPS, FILE_MODIFIED);
    sdFile5.vReadCheckFile(LOOPS, FILE_MODIFIED);
    
    if (!LOG_DETAIL)
    {
        memset(msg, 0, sizeof(msg));
        sprintf(msg, "Pass: %d", nPass);
        TLIB_LOG(msg);
        memset(msg, 0, sizeof(msg));
        sprintf(msg, "Fail: %d", nFail);
        TLIB_LOG(msg);
        TLIB_ASSERT(nFail == 0);
    }
    TLIB_SUITE_END("Testing Multiple File Open Write");
} 

void vTest_MultiFileWrite(void)
{
    int i = 0;
    CSDFile sdFileA("FILEA.DAT");
    CSDFile sdFileB("FILEB.DAT");
    CSDFile sdFileC("FILEC.DAT");
    CSDFile sdFileD("FILED.DAT");
    CSDFile sdFileE("FILEE.DAT");
    CSDFile sdFileF("FILEF.DAT");
    CSDFile sdFileG("FILEG.DAT");
    CSDFile sdFileH("FILEH.DAT");
    CSDFile sdFileI("FILEI.DAT");
    CSDFile sdFileJ("FILEJ.DAT");

    TLIB_SUITE("Testing Multiple File Write");
    clrscr();
    nFail = 0;
    nPass = 0;
    
    //create 10 files
    sdFileA.vCreateFile();
    sdFileB.vCreateFile();
    sdFileC.vCreateFile();
    sdFileD.vCreateFile();
    sdFileE.vCreateFile();
    sdFileF.vCreateFile();
    sdFileG.vCreateFile();
    sdFileH.vCreateFile();
    sdFileI.vCreateFile();
    sdFileJ.vCreateFile();
    
    for ( i = 1; i <= LOOPS; i++)
    {
        gotoxy(1, 1);
        clreol();
        printf("TEST 2/%d\n", TESTCASES);
        printf("PROGRESS: %d%%  \n", ((i * 100)/LOOPS));
        
        //open files
        sdFileA.vOpen(O_RDWR, 0, 0);
        sdFileB.vOpen(O_RDWR, 0, 0);
        sdFileC.vOpen(O_RDWR, 0, 0);
        sdFileD.vOpen(O_RDWR, 0, 0);
        sdFileE.vOpen(O_RDWR, 0, 0);
        sdFileF.vOpen(O_RDWR, 0, 0);
        sdFileG.vOpen(O_RDWR, 0, 0);
        sdFileH.vOpen(O_RDWR, 0, 0);
        sdFileI.vOpen(O_RDWR, 0, 0);
        sdFileJ.vOpen(O_RDWR, 0, 0);
        
        //lseek files
        sdFileA.vLSeek(0, SEEK_END, 0, 0);
        sdFileB.vLSeek(0, SEEK_END, 0, 0);
        sdFileC.vLSeek(0, SEEK_END, 0, 0);
        sdFileD.vLSeek(0, SEEK_END, 0, 0);
        sdFileE.vLSeek(0, SEEK_END, 0, 0);
        sdFileF.vLSeek(0, SEEK_END, 0, 0);
        sdFileG.vLSeek(0, SEEK_END, 0, 0);
        sdFileH.vLSeek(0, SEEK_END, 0, 0);
        sdFileI.vLSeek(0, SEEK_END, 0, 0);
        sdFileJ.vLSeek(0, SEEK_END, 0, 0);
        
        //write
        sdFileA.vWrite(i, 0, 0);
        sdFileB.vWrite(i, 0, 0);
        sdFileC.vWrite(i, 0, 0);
        sdFileD.vWrite(i, 0, 0);
        sdFileE.vWrite(i, 0, 0);
        sdFileF.vWrite(i, 0, 0);
        sdFileG.vWrite(i, 0, 0);
        sdFileH.vWrite(i, 0, 0);
        sdFileI.vWrite(i, 0, 0);
        sdFileJ.vWrite(i, 0, 0);
        
        //close
        sdFileA.vClose();
        sdFileB.vClose();
        sdFileC.vClose();
        sdFileD.vClose();
        sdFileE.vClose();
        sdFileF.vClose();
        sdFileG.vClose();
        sdFileH.vClose();
        sdFileI.vClose();
        sdFileJ.vClose();
    }    
    
    //modify                   
    sdFileA.vModify();
    sdFileB.vModify();
    sdFileC.vModify();
    sdFileD.vModify();
    sdFileE.vModify();
    sdFileF.vModify();
    sdFileG.vModify();
    sdFileH.vModify();
    sdFileI.vModify();
    sdFileJ.vModify();
                           
    //check files
    clrscr();               
    sdFileA.vReadCheckFile(LOOPS, FILE_MODIFIED);
    sdFileB.vReadCheckFile(LOOPS, FILE_MODIFIED);
    sdFileC.vReadCheckFile(LOOPS, FILE_MODIFIED);
    sdFileD.vReadCheckFile(LOOPS, FILE_MODIFIED);
    sdFileE.vReadCheckFile(LOOPS, FILE_MODIFIED);
    sdFileF.vReadCheckFile(LOOPS, FILE_MODIFIED);
    sdFileG.vReadCheckFile(LOOPS, FILE_MODIFIED);
    sdFileH.vReadCheckFile(LOOPS, FILE_MODIFIED);
    sdFileI.vReadCheckFile(LOOPS, FILE_MODIFIED);
    sdFileJ.vReadCheckFile(LOOPS, FILE_MODIFIED);
    
    if (!LOG_DETAIL)
    {
        memset(msg, 0, sizeof(msg));
        sprintf(msg, "Pass: %d", nPass);
        TLIB_LOG(msg);
        memset(msg, 0, sizeof(msg));
        sprintf(msg, "Fail: %d", nFail);
        TLIB_LOG(msg);
        TLIB_ASSERT(nFail == 0);
    }
    TLIB_SUITE_END("Testing Multiple File Write");
} 
 
void vTest_MultiCreateFile(void)
{
    int i = 0;
    int j = 0;
    int k = 0;
    int l = 0;
    CSDFile sdFile11("FILE11.DAT"); 
    CSDFile sdFile12("FILE12.DAT"); 
    CSDFile sdFile13("FILE13.DAT"); 
    CSDFile sdFile14("FILE14.DAT"); 
    CSDFile sdFile15("FILE15.DAT"); 
    CSDFile sdFile16("FILE16.DAT"); 
    CSDFile sdFile17("FILE17.DAT"); 

    TLIB_SUITE("Testing Multiple File Creation");
    clrscr();
    nFail = 0;
    nPass = 0;
    
    sdFile11.vCreateFile(); 
    sdFile12.vCreateFile();
    sdFile13.vCreateFile();
    sdFile14.vCreateFile();
            
    for ( i = 1; i <= 10; i++ )
    {
        sdFile11.vWriteToFile(i);
        sdFile15.vCreateFile();
        
        //write to files
        for ( j = 1; j <= 10; j++)
        {
            gotoxy(1, 1);
            clreol();
            printf("TEST 3/%d\n", TESTCASES);
            printf("PROGRESS: %d%%  \n", (10 * (i-1)) + j );
            
            sdFile12.vWriteToFile((10 * (i-1)) + j);
            sdFile16.vCreateFile();
        
            for ( k = 1; k <= 10; k++)
            {
                sdFile13.vWriteToFile((100 * (i-1)) + (10 * (j-1)) + k);
                sdFile17.vCreateFile();
                
                for ( l = 1; l <= 10; l++ )
                {
                    sdFile14.vWriteToFile((1000 * (i-1)) + (100 * (j-1)) + (10 * (k-1)) + l);
                }     
                
                for ( l = 1; l <= 10; l++ )
                {
                    sdFile17.vWriteToFile(l); 
                }         
            }
            
            for ( k = 1; k <= 10; k++)
            {
                sdFile16.vWriteToFile(k);
            }
        }
           
        for ( j = 1; j <= 10; j++)
        {
            sdFile15.vWriteToFile(j);
        }
    }
    
    //check files 
    clrscr();             
    sdFile11.vReadCheckFile(10, FILE_NONMODIFIED);
    sdFile12.vReadCheckFile(100, FILE_NONMODIFIED);
    sdFile13.vReadCheckFile(1000, FILE_NONMODIFIED);
    sdFile14.vReadCheckFile(10000, FILE_NONMODIFIED);
    sdFile15.vReadCheckFile(10, FILE_NONMODIFIED);
    sdFile16.vReadCheckFile(10, FILE_NONMODIFIED);
    sdFile17.vReadCheckFile(10, FILE_NONMODIFIED);
    
    if (!LOG_DETAIL)
    {           
        memset(msg, 0, sizeof(msg));
        sprintf(msg, "Pass: %d", nPass);
        TLIB_LOG(msg);
        memset(msg, 0, sizeof(msg));
        sprintf(msg, "Fail: %d", nFail);
        TLIB_LOG(msg);
        TLIB_ASSERT(nFail == 0);
    }
    TLIB_SUITE_END("Testing Multiple File Creation");
}

void vTest_OpenAttributes(void)
{
    int i = 0;
    int j = 0;
    int k = 0;
    int l = 0;
    char buff[2] = "";
    int nRun = 0;
    CSDFile sdFile21("FILE21.DAT");
    CSDFile sdFile22("FILE22.DAT");
    CSDFile sdFile23("FILE23.DAT");
    CSDFile sdFile24("FILE24.DAT");
    CSDFile sdFile25("FILE25.DAT");
    CSDFile sdFile26("FILE26.DAT");
    CSDFile sdFile27("FILE27.DAT");
    CSDFile sdFile28("FILE28.DAT");
    CSDFile sdFile29("FILE29.DAT");
    
    TLIB_SUITE("Testing Open File Attributes");
    clrscr();
    nFail = 0;
    nPass = 0;
    
    get_env("PASS", buff, sizeof(buff));
    nRun = atoi(buff);
    if (nRun == 1)
    {    
        //attributes on non-existent file
        sdFile21.vOpen(O_RDONLY, -1, ENOENT);
        sdFile21.vOpen(O_WRONLY, -1, ENOENT);
        sdFile21.vOpen(O_RDWR, -1, ENOENT);
        sdFile21.vOpen(O_APPEND, -1, ENOENT);    
        sdFile21.vOpen(O_TRUNC, -1, ENOENT);
    }
    
    //file already exists
    sdFile21.vOpen(O_CREAT, 0, 0);
    sdFile21.vOpen(O_CREAT | O_EXCL, -1, 17);
    
    //attributes on existing file with different write intervals
    sdFile22.vCreateFile();
    sdFile23.vCreateFile();
    sdFile24.vCreateFile();
    sdFile25.vCreateFile();
    sdFile26.vCreateFile();
    sdFile27.vCreateFile();
    sdFile28.vCreateFile();
    sdFile29.vCreateFile();
    
    sdFile21.vOpen(O_TRUNC | O_RDWR, 0, 0);
    sdFile22.vOpen(O_TRUNC | O_WRONLY, 0, 0);
    sdFile23.vOpen(O_APPEND | O_RDWR, 0, 0);
    sdFile24.vOpen(O_APPEND | O_WRONLY, 0, 0);
    sdFile25.vOpen(O_WRONLY, 0, 0);
    sdFile26.vOpen(O_RDWR, 0, 0);
    
    for ( i = 1; i <= 10; i++ )
    {
        sdFile21.vWrite(i, 0, 0);
        sdFile22.vWrite(i, 0, 0);
        
        sdFile27.vOpen(O_TRUNC | O_WRONLY, 0, 0);
        
        for ( j = 1; j <= 10; j++)
        {
            gotoxy(1, 1);
            clreol();
            printf("TEST 4/%d\n", TESTCASES);
            printf("PROGRESS: %d%%  \n", (10 * (i-1)) + j );
            
            sdFile23.vWrite((10 * (i-1)) + j, 0, 0);
            sdFile24.vWrite((10 * (i-1)) + j, 0, 0);
            sdFile28.vOpen(O_APPEND | O_RDWR, 0, 0);
            
            for ( k = 1; k <= 10; k++)
            {
                sdFile25.vWrite((100 * (i-1)) + (10 * (j-1)) + k, 0, 0);  
                sdFile29.vOpen(O_WRONLY, 0, 0);
                
                for ( l = 1; l <= 10; l++ )
                {
                    sdFile26.vWrite((1000 * (i-1)) + (100 * (j-1)) + (10 * (k-1)) + l, 0, 0); 
                }     
                
                for ( l = 1; l <= 10; l++ )
                {
                    sdFile29.vWrite(l, 0, 0);
                }      
                sdFile29.vClose();
            }
            for ( k = 1; k <= 10; k++)
            {
                sdFile28.vWrite((100 * (i-1)) + (10 * (j-1)) + k, 0, 0);
            }
            sdFile28.vClose();
        }
        
        for ( j = 1; j <= 10; j++)
        {
            sdFile27.vWrite(j, 0, 0);
        }
        
        sdFile27.vClose();
    }
    
    sdFile21.vClose();
    sdFile22.vClose();
    sdFile23.vClose();
    sdFile24.vClose();
    sdFile25.vClose();  
    sdFile26.vClose();
    
    //check files 
    clrscr(); 
    sdFile21.vReadCheckFile(10, FILE_NONMODIFIED);
    sdFile22.vReadCheckFile(10, FILE_NONMODIFIED); 
    sdFile23.vReadCheckFile(100, FILE_NONMODIFIED); 
    sdFile24.vReadCheckFile(100, FILE_NONMODIFIED); 
    sdFile25.vReadCheckFile(1000, FILE_NONMODIFIED); 
    sdFile26.vReadCheckFile(10000, FILE_NONMODIFIED); 
    sdFile27.vReadCheckFile(10, FILE_NONMODIFIED); 
    sdFile28.vReadCheckFile(1000, FILE_NONMODIFIED); 
    sdFile29.vReadCheckFile(10, FILE_NONMODIFIED); 
    
    if (!LOG_DETAIL)
    {           
        memset(msg, 0, sizeof(msg));
        sprintf(msg, "Pass: %d", nPass);
        TLIB_LOG(msg);
        memset(msg, 0, sizeof(msg));
        sprintf(msg, "Fail: %d", nFail);
        TLIB_LOG(msg);
        TLIB_ASSERT(nFail == 0);
    }
    TLIB_SUITE_END("Testing Open File Attributes");
}

void vTest_OpenAttributesNeg(void)
{
    int i = 0;
    int j = 0;
    int k = 0;
    int l = 0;
    CSDFile sdFile31("FILE31.DAT");
    CSDFile sdFile32("FILE32.DAT");
    CSDFile sdFile33("FILE33.DAT");
    CSDFile sdFile34("FILE34.DAT");
    CSDFile sdFile35("FILE35.DAT");
    CSDFile sdFile36("FILE36.DAT");
    CSDFile sdFile37("FILE37.DAT");
    CSDFile sdFile38("FILE38.DAT");
    CSDFile sdFile39("FILE39.DAT");
    
    TLIB_SUITE("Testing Open File Attributes Negative Cases");
    clrscr();
    nFail = 0;
    nPass = 0;
    
    //attributes on existing file with different write intervals
    sdFile31.vCreateFile();
    sdFile32.vCreateFile();
    sdFile33.vCreateFile();
    sdFile34.vCreateFile();
    sdFile35.vCreateFile();
    sdFile36.vCreateFile();
    sdFile37.vCreateFile();
    sdFile38.vCreateFile();
    sdFile39.vCreateFile();
    
    sdFile31.vOpen(O_TRUNC | O_RDONLY, 0, 0);
    sdFile32.vOpen(O_TRUNC, 0, 0);
    sdFile33.vOpen(O_APPEND | O_RDONLY, 0, 0);
    sdFile34.vOpen(O_APPEND, 0, 0);
    sdFile35.vOpen(O_RDONLY, 0, 0);
    
    for ( i = 1; i <= 10; i++ )
    {
        sdFile31.vWrite(i, -1, EACCES);
        sdFile36.vOpen(O_TRUNC | O_RDONLY, 0, 0);
        
        for ( j = 1; j <= 10; j++)
        {
            gotoxy(1, 1);
            clreol();
            printf("TEST 5/%d\n", TESTCASES);
            printf("PROGRESS: %d%%  \n", (10 * (i-1)) + j );
            
            sdFile32.vWrite((10 * (i-1)) + j, -1, EACCES);
            sdFile33.vWrite((10 * (i-1)) + j, -1, EACCES);
            sdFile37.vOpen(O_APPEND | O_RDONLY, 0, 0);
            
            for ( k = 1; k <= 10; k++)
            {
                sdFile34.vWrite((100 * (i-1)) + (10 * (j-1)) + k, -1, EACCES);  
                sdFile38.vOpen(O_RDONLY, 0, 0);
                
                for ( l = 1; l <= 10; l++ )
                {
                    sdFile35.vWrite((1000 * (i-1)) + (100 * (j-1)) + (10 * (k-1)) + l, -1, EACCES); 
                }     
                
                for ( l = 1; l <= 10; l++ )
                {
                    sdFile38.vWrite(l, -1, EACCES);
                }      
                sdFile38.vClose();
            }
            for ( k = 1; k <= 10; k++)
            {
                sdFile37.vWrite(k, -1, EACCES);
            }
            sdFile37.vClose();
        }
        
        for ( j = 1; j <= 10; j++)
        {
            sdFile36.vWrite(j, -1, EACCES);
        }
        
        sdFile36.vClose();
    }

    sdFile31.vClose();
    sdFile32.vClose();
    sdFile33.vClose();
    sdFile34.vClose();
    sdFile35.vClose();  
    
    if (!LOG_DETAIL)
    {           
        memset(msg, 0, sizeof(msg));
        sprintf(msg, "Pass: %d", nPass);
        TLIB_LOG(msg);
        memset(msg, 0, sizeof(msg));
        sprintf(msg, "Fail: %d", nFail);
        TLIB_LOG(msg);
        TLIB_ASSERT(nFail == 0);
    }
    TLIB_SUITE_END("Testing Open File Attributes Negative Cases");
}

void vTest_SeekSetTest(void)
{
    int i = 0; 
    int j = 0; 
    int k = 0; 
    int l = 0; 
    int linenumber = 0;
    CSDFile sdFileA1("FILEA1.DAT");
    CSDFile sdFileA2("FILEA2.DAT");
    CSDFile sdFileA3("FILEA3.DAT");
    CSDFile sdFileA4("FILEA4.DAT");
    CSDFile sdFileB1("FILEB1.DAT");
    CSDFile sdFileB2("FILEB2.DAT");
    CSDFile sdFileB3("FILEB3.DAT");
    CSDFile sdFileB4("FILEB4.DAT");
       
    TLIB_SUITE("Testing LSEEK with SEEK_SET");
    clrscr();
    gotoxy(1, 3);
    printf("LSEEK Test");
    nFail = 0;
    nPass = 0;
    
    sdFileA1.vOpen(O_RDONLY, 0, 0);                                       
    sdFileA2.vOpen(O_RDONLY, 0, 0);
    sdFileA3.vOpen(O_RDONLY, 0, 0);  
    sdFileA4.vOpen(O_RDONLY, 0, 0); 
    sdFileB1.vOpen(O_WRONLY, 0, 0);                                       
    sdFileB2.vOpen(O_WRONLY, 0, 0);
    sdFileB3.vOpen(O_WRONLY, 0, 0);  
    sdFileB4.vOpen(O_WRONLY, 0, 0); 
    
    //SEEK_SET test
    for ( i = 1; i <= 10; i++ )
    {
        sdFileA1.vLSeek((i - 1) * sdFileA1.nRecordLen, SEEK_SET, (i - 1) * sdFileA1.nRecordLen, 0);
        sdFileA1.vRead(i, sdFileA1.nRecordLen, 0); 
        sdFileB1.vLSeek((i - 1) * sdFileB1.nRecordLen, SEEK_SET, (i - 1) * sdFileB1.nRecordLen, 0);
        sdFileB1.vWrite(i, sdFileB1.nRecordLen, 0); 
        for ( j = 1; j <= 10; j++ )
        {
            linenumber = (10 * (i - 1)) + j;
            gotoxy(1, 1);
            clreol();
            printf("TEST 6/%d\n", TESTCASES);
            printf("PROGRESS: %d%%  \n", linenumber );
            
            sdFileA2.vLSeek((linenumber - 1) * sdFileA2.nRecordLen, SEEK_SET, (linenumber - 1) * sdFileA2.nRecordLen, 0);
            sdFileA2.vRead(linenumber, sdFileA2.nRecordLen, 0); 
            sdFileB2.vLSeek((linenumber - 1) * sdFileB2.nRecordLen, SEEK_SET, (linenumber - 1) * sdFileB2.nRecordLen, 0);
            sdFileB2.vWrite(linenumber, sdFileB2.nRecordLen, 0); 
            
            for ( k = 1; k <= 10; k++)
            {
                linenumber = (100 * (i-1)) + (10 * (j-1)) + k;
                sdFileA3.vLSeek((linenumber - 1) * sdFileA3.nRecordLen, SEEK_SET, (linenumber - 1) * sdFileA3.nRecordLen, 0);
                sdFileA3.vRead(linenumber, sdFileA3.nRecordLen, 0); 
                sdFileB3.vLSeek((linenumber - 1) * sdFileB3.nRecordLen, SEEK_SET, (linenumber - 1) * sdFileB3.nRecordLen, 0);
                sdFileB3.vWrite(linenumber, sdFileB3.nRecordLen, 0); 
                
                for ( l = 1; l <= 10; l++ )
                {
                    linenumber = (1000 * (i-1)) + (100 * (j-1)) + (10 * (k-1)) + l;
                    sdFileA4.vLSeek((linenumber - 1) * sdFileA4.nRecordLen, SEEK_SET, (linenumber - 1) * sdFileA4.nRecordLen, 0);
                    sdFileA4.vRead(linenumber, sdFileA4.nRecordLen, 0); 
                    sdFileB4.vLSeek((linenumber - 1) * sdFileB4.nRecordLen, SEEK_SET, (linenumber - 1) * sdFileB4.nRecordLen, 0);
                    sdFileB4.vWrite(linenumber, sdFileB4.nRecordLen, 0); 
                }
            }
        }
    }
    sdFileA1.vClose(); 
    sdFileA2.vClose(); 
    sdFileA3.vClose(); 
    sdFileA4.vClose(); 
    sdFileB1.vClose(); 
    sdFileB2.vClose(); 
    sdFileB3.vClose(); 
    sdFileB4.vClose(); 
    
    //check files 
    clrscr(); 
    sdFileB1.vReadCheckFile(10, FILE_NONMODIFIED);
    sdFileB2.vReadCheckFile(100, FILE_NONMODIFIED); 
    sdFileB3.vReadCheckFile(1000, FILE_NONMODIFIED); 
    sdFileB4.vReadCheckFile(10000, FILE_NONMODIFIED); 
    
    if (!LOG_DETAIL)
    {           
        memset(msg, 0, sizeof(msg));
        sprintf(msg, "Pass: %d", nPass);
        TLIB_LOG(msg);
        memset(msg, 0, sizeof(msg));
        sprintf(msg, "Fail: %d", nFail);
        TLIB_LOG(msg);
        TLIB_ASSERT(nFail == 0);
    }
    TLIB_SUITE_END("Testing LSEEK with SEEK_SET");
}

void vTest_SeekCurFromSetTest(void)
{
    int i = 0; 
    int j = 0; 
    int k = 0; 
    int l = 0; 
    int linenumber = 0;
    CSDFile sdFileA1("FILEA1.DAT");
    CSDFile sdFileA2("FILEA2.DAT");
    CSDFile sdFileA3("FILEA3.DAT");
    CSDFile sdFileA4("FILEA4.DAT");
    CSDFile sdFileC1("FILEC1.DAT");
    CSDFile sdFileC2("FILEC2.DAT");
    CSDFile sdFileC3("FILEC3.DAT");
    CSDFile sdFileC4("FILEC4.DAT");
       
    TLIB_SUITE("Testing LSEEK with SEEK_CUR");
    clrscr();
    gotoxy(1, 3);
    printf("LSEEK Test");
    nFail = 0;
    nPass = 0;
    
    sdFileA1.vOpen(O_RDONLY, 0, 0);                                       
    sdFileA2.vOpen(O_RDONLY, 0, 0);
    sdFileA3.vOpen(O_RDONLY, 0, 0);  
    sdFileA4.vOpen(O_RDONLY, 0, 0);
    sdFileC1.vOpen(O_RDWR, 0, 0);                                       
    sdFileC2.vOpen(O_RDWR, 0, 0);
    sdFileC3.vOpen(O_RDWR, 0, 0);  
    sdFileC4.vOpen(O_RDWR, 0, 0);
    
    for ( i = 1; i <= 10; i++ )
    {
        sdFileA1.vLSeek( 0, SEEK_SET, 0, 0);
        sdFileA1.vLSeek((i - 1) * sdFileA1.nRecordLen, SEEK_CUR, (i - 1) * sdFileA1.nRecordLen, 0);
        sdFileA1.vRead(i, sdFileA1.nRecordLen, 0); 
        sdFileC1.vLSeek( 0, SEEK_SET, 0, 0);
        sdFileC1.vLSeek((i - 1) * sdFileC1.nRecordLen, SEEK_SET, (i - 1) * sdFileC1.nRecordLen, 0);
        sdFileC1.vWrite(i, sdFileC1.nRecordLen, 0); 
        for ( j = 1; j <= 10; j++ )
        {
            linenumber = (10 * (i - 1)) + j;
            gotoxy(1, 1);
            clreol();
            printf("TEST 7/%d\n", TESTCASES);
            printf("PROGRESS: %d%%  \n", linenumber );
            
            sdFileA2.vLSeek( 0, SEEK_SET, 0, 0);
            sdFileA2.vLSeek((linenumber - 1) * sdFileA2.nRecordLen, SEEK_CUR, (linenumber - 1) * sdFileA2.nRecordLen, 0);
            sdFileA2.vRead(linenumber, sdFileA2.nRecordLen, 0); 
            sdFileC2.vLSeek( 0, SEEK_SET, 0, 0);
            sdFileC2.vLSeek((linenumber - 1) * sdFileC2.nRecordLen, SEEK_CUR, (linenumber - 1) * sdFileC2.nRecordLen, 0);
            sdFileC2.vWrite(linenumber, sdFileC2.nRecordLen, 0); 
            
            for ( k = 1; k <= 10; k++)
            {
                linenumber = (100 * (i-1)) + (10 * (j-1)) + k;
                sdFileA3.vLSeek( 0, SEEK_SET, 0, 0);
                sdFileA3.vLSeek((linenumber - 1) * sdFileA3.nRecordLen, SEEK_CUR, (linenumber - 1) * sdFileA3.nRecordLen, 0);
                sdFileA3.vRead(linenumber, sdFileA3.nRecordLen, 0); 
                sdFileC3.vLSeek( 0, SEEK_SET, 0, 0);
                sdFileC3.vLSeek((linenumber - 1) * sdFileC3.nRecordLen, SEEK_CUR, (linenumber - 1) * sdFileC3.nRecordLen, 0);
                sdFileC3.vWrite(linenumber, sdFileC3.nRecordLen, 0); 
                
                for ( l = 1; l <= 10; l++ )
                {
                    linenumber = (1000 * (i-1)) + (100 * (j-1)) + (10 * (k-1)) + l;
                    sdFileA4.vLSeek( 0, SEEK_SET, 0, 0);
                    sdFileA4.vLSeek((linenumber - 1) * sdFileA4.nRecordLen, SEEK_CUR, (linenumber - 1) * sdFileA4.nRecordLen, 0);
                    sdFileA4.vRead(linenumber, sdFileA4.nRecordLen, 0); 
                    sdFileC4.vLSeek( 0, SEEK_SET, 0, 0);
                    sdFileC4.vLSeek((linenumber - 1) * sdFileC4.nRecordLen, SEEK_CUR, (linenumber - 1) * sdFileC4.nRecordLen, 0);
                    sdFileC4.vWrite(linenumber, sdFileC4.nRecordLen, 0); 
                }
            }
        }
    } 
    sdFileA1.vClose(); 
    sdFileA2.vClose(); 
    sdFileA3.vClose(); 
    sdFileA4.vClose(); 
    sdFileC1.vClose(); 
    sdFileC2.vClose(); 
    sdFileC3.vClose(); 
    sdFileC4.vClose();
    
    //check files 
    clrscr(); 
    sdFileC1.vReadCheckFile(10, FILE_NONMODIFIED);
    sdFileC2.vReadCheckFile(100, FILE_NONMODIFIED); 
    sdFileC3.vReadCheckFile(1000, FILE_NONMODIFIED); 
    sdFileC4.vReadCheckFile(10000, FILE_NONMODIFIED); 
    
    if (!LOG_DETAIL)
    {           
        memset(msg, 0, sizeof(msg));
        sprintf(msg, "Pass: %d", nPass);
        TLIB_LOG(msg);
        memset(msg, 0, sizeof(msg));
        sprintf(msg, "Fail: %d", nFail);
        TLIB_LOG(msg);
        TLIB_ASSERT(nFail == 0);
    }
    TLIB_SUITE_END("Testing LSEEK with SEEK_CUR");
}

void vTest_SeekCurFromEndTest(void)
{
    int i = 0; 
    int j = 0; 
    int k = 0; 
    int l = 0; 
    int linenumber = 0;
    CSDFile sdFileA1("FILEA1.DAT");
    CSDFile sdFileA2("FILEA2.DAT");
    CSDFile sdFileA3("FILEA3.DAT");
    CSDFile sdFileA4("FILEA4.DAT");
    CSDFile sdFileD1("FILED1.DAT");
    CSDFile sdFileD2("FILED2.DAT");
    CSDFile sdFileD3("FILED3.DAT");
    CSDFile sdFileD4("FILED4.DAT");
       
    TLIB_SUITE("Testing LSEEK with SEEK_CUR");
    clrscr();
    gotoxy(1, 3);
    printf("LSEEK Test");
    nFail = 0;
    nPass = 0;
    
    sdFileA1.vOpen(O_RDONLY, 0, 0);                                       
    sdFileA2.vOpen(O_RDONLY, 0, 0);
    sdFileA3.vOpen(O_RDONLY, 0, 0);  
    sdFileA4.vOpen(O_RDONLY, 0, 0); 
    sdFileD1.vOpen(O_WRONLY, 0, 0);                                       
    sdFileD2.vOpen(O_WRONLY, 0, 0);
    sdFileD3.vOpen(O_WRONLY, 0, 0);  
    sdFileD4.vOpen(O_WRONLY, 0, 0); 
    
    for ( i = 1; i <= 10; i++ )
    {                   
        sdFileA1.vLSeek(0, SEEK_END, 0, 0);     
        sdFileA1.vLSeek(-(10 - (i - 1)) * sdFileA1.nRecordLen, SEEK_CUR, (i - 1)* sdFileA1.nRecordLen, 0);
        sdFileA1.vRead(i, sdFileA1.nRecordLen, 0); 
        sdFileD1.vLSeek(0, SEEK_END, 0, 0);     
        sdFileD1.vLSeek(-(10 - (i - 1)) * sdFileD1.nRecordLen, SEEK_CUR, (i - 1)* sdFileD1.nRecordLen, 0);
        sdFileD1.vWrite(i, sdFileD1.nRecordLen, 0);
        for ( j = 1; j <= 10; j++ )
        {
            linenumber = (10 * (i - 1)) + j;
            gotoxy(1, 1);
            clreol();
            printf("TEST 8/%d\n", TESTCASES);
            printf("PROGRESS: %d%%  \n", linenumber );
                                    
            sdFileA2.vLSeek(0, SEEK_END, 0, 0);
            sdFileA2.vLSeek(-(100 - (linenumber - 1)) * sdFileA2.nRecordLen, SEEK_CUR, (linenumber - 1) * sdFileA2.nRecordLen, 0);
            sdFileA2.vRead(linenumber, sdFileA2.nRecordLen, 0); 
            sdFileD2.vLSeek(0, SEEK_END, 0, 0);
            sdFileD2.vLSeek(-(100 - (linenumber - 1)) * sdFileD2.nRecordLen, SEEK_CUR, (linenumber - 1) * sdFileD2.nRecordLen, 0);
            sdFileD2.vWrite(linenumber, sdFileD2.nRecordLen, 0); 
            
            for ( k = 1; k <= 10; k++)
            {
                linenumber = (100 * (i-1)) + (10 * (j-1)) + k;
                sdFileA3.vLSeek(0, SEEK_END, 0, 0);
                sdFileA3.vLSeek(-(1000 - (linenumber - 1)) * sdFileA3.nRecordLen, SEEK_CUR, (linenumber - 1) * sdFileA3.nRecordLen, 0);
                sdFileA3.vRead(linenumber, sdFileA3.nRecordLen, 0); 
                sdFileD3.vLSeek(0, SEEK_END, 0, 0);
                sdFileD3.vLSeek(-(1000 - (linenumber - 1)) * sdFileD3.nRecordLen, SEEK_CUR, (linenumber - 1) * sdFileD3.nRecordLen, 0);
                sdFileD3.vWrite(linenumber, sdFileD3.nRecordLen, 0); 
                
                for ( l = 1; l <= 10; l++ )
                {
                    linenumber = (1000 * (i-1)) + (100 * (j-1)) + (10 * (k-1)) + l;
                    sdFileA4.vLSeek(0, SEEK_END, 0, 0);
                    sdFileA4.vLSeek(-(10000 - (linenumber - 1)) * sdFileA4.nRecordLen, SEEK_CUR, (linenumber - 1) * sdFileA4.nRecordLen, 0);
                    sdFileA4.vRead(linenumber, sdFileA4.nRecordLen, 0); 
                    sdFileD4.vLSeek(0, SEEK_END, 0, 0);
                    sdFileD4.vLSeek(-(10000 - (linenumber - 1)) * sdFileD4.nRecordLen, SEEK_CUR, (linenumber - 1) * sdFileD4.nRecordLen, 0);
                    sdFileD4.vWrite(linenumber, sdFileD4.nRecordLen, 0); 
                }
            }
        }
    } 
    sdFileA1.vClose(); 
    sdFileA2.vClose(); 
    sdFileA3.vClose(); 
    sdFileA4.vClose(); 
    sdFileD1.vClose(); 
    sdFileD2.vClose(); 
    sdFileD3.vClose(); 
    sdFileD4.vClose();
    
    //check files 
    clrscr(); 
    sdFileD1.vReadCheckFile(10, FILE_NONMODIFIED);
    sdFileD2.vReadCheckFile(100, FILE_NONMODIFIED); 
    sdFileD3.vReadCheckFile(1000, FILE_NONMODIFIED); 
    sdFileD4.vReadCheckFile(10000, FILE_NONMODIFIED); 
    
    if (!LOG_DETAIL)
    {           
        memset(msg, 0, sizeof(msg));
        sprintf(msg, "Pass: %d", nPass);
        TLIB_LOG(msg);
        memset(msg, 0, sizeof(msg));
        sprintf(msg, "Fail: %d", nFail);
        TLIB_LOG(msg);
        TLIB_ASSERT(nFail == 0);
    }
    TLIB_SUITE_END("Testing LSEEK with SEEK_CUR");
}
    
void vTest_SeekEndTest(void)
{   
    int i = 0; 
    int j = 0; 
    int k = 0; 
    int l = 0; 
    int linenumber = 0;
    CSDFile sdFileA1("FILEA1.DAT");
    CSDFile sdFileA2("FILEA2.DAT");
    CSDFile sdFileA3("FILEA3.DAT");
    CSDFile sdFileA4("FILEA4.DAT");
    CSDFile sdFileE1("FILEE1.DAT");
    CSDFile sdFileE2("FILEE2.DAT");
    CSDFile sdFileE3("FILEE3.DAT");
    CSDFile sdFileE4("FILEE4.DAT");
       
    TLIB_SUITE("Testing LSEEK with SEEK_END");
    clrscr();
    gotoxy(1, 3);
    printf("LSEEK Test");
    nFail = 0;
    nPass = 0;
                                  
    sdFileA1.vOpen(O_RDONLY, 0, 0);                                       
    sdFileA2.vOpen(O_RDONLY, 0, 0);
    sdFileA3.vOpen(O_RDONLY, 0, 0);  
    sdFileA4.vOpen(O_RDONLY, 0, 0); 
    sdFileE1.vOpen(O_RDWR, 0, 0); 
    sdFileE2.vOpen(O_RDWR, 0, 0); 
    sdFileE3.vOpen(O_RDWR, 0, 0); 
    sdFileE4.vOpen(O_RDWR, 0, 0); 
 
    
    for ( i = 1; i <= 10; i++ )
    {                        
        sdFileA1.vLSeek(-(10 - (i - 1)) * sdFileA1.nRecordLen, SEEK_END, (i - 1)* sdFileA1.nRecordLen, 0);
        sdFileA1.vRead(i, sdFileA1.nRecordLen, 0); 
        sdFileE1.vLSeek(-(10 - (i - 1)) * sdFileE1.nRecordLen, SEEK_END, (i - 1)* sdFileE1.nRecordLen, 0);
        sdFileE1.vWrite(i, sdFileE1.nRecordLen, 0); 
        for ( j = 1; j <= 10; j++ )
        {
            linenumber = (10 * (i - 1)) + j;
            gotoxy(1, 1);
            clreol();
            printf("TEST 9/%d\n", TESTCASES);
            printf("PROGRESS: %d%%  \n", linenumber );
            
            sdFileA2.vLSeek(-(100 - (linenumber - 1)) * sdFileA2.nRecordLen, SEEK_END, (linenumber - 1) * sdFileA2.nRecordLen, 0);
            sdFileA2.vRead(linenumber, sdFileA2.nRecordLen, 0); 
            sdFileE2.vLSeek(-(100 - (linenumber - 1)) * sdFileE2.nRecordLen, SEEK_END, (linenumber - 1) * sdFileE2.nRecordLen, 0);
            sdFileE2.vWrite(linenumber, sdFileE2.nRecordLen, 0); 
            
            for ( k = 1; k <= 10; k++)
            {
                linenumber = (100 * (i-1)) + (10 * (j-1)) + k;
                sdFileA3.vLSeek(-(1000 - (linenumber - 1)) * sdFileA3.nRecordLen, SEEK_END, (linenumber - 1) * sdFileA3.nRecordLen, 0);
                sdFileA3.vRead(linenumber, sdFileA3.nRecordLen, 0); 
                sdFileE3.vLSeek(-(1000 - (linenumber - 1)) * sdFileE3.nRecordLen, SEEK_END, (linenumber - 1) * sdFileE3.nRecordLen, 0);
                sdFileE3.vWrite(linenumber, sdFileE3.nRecordLen, 0); 
                
                for ( l = 1; l <= 10; l++ )
                {
                    linenumber = (1000 * (i-1)) + (100 * (j-1)) + (10 * (k-1)) + l;
                    sdFileA4.vLSeek(-(10000 - (linenumber - 1)) * sdFileA4.nRecordLen, SEEK_END, (linenumber - 1) * sdFileA4.nRecordLen, 0);
                    sdFileA4.vRead(linenumber, sdFileA4.nRecordLen, 0); 
                    sdFileE4.vLSeek(-(10000 - (linenumber - 1)) * sdFileE4.nRecordLen, SEEK_END, (linenumber - 1) * sdFileE4.nRecordLen, 0);
                    sdFileE4.vWrite(linenumber, sdFileE4.nRecordLen, 0);
                }
            }
        }
    } 
    
    sdFileA1.vClose(); 
    sdFileA2.vClose(); 
    sdFileA3.vClose(); 
    sdFileA4.vClose();
    sdFileE1.vClose(); 
    sdFileE2.vClose(); 
    sdFileE3.vClose(); 
    sdFileE4.vClose(); 
    
    //check files 
    clrscr(); 
    sdFileE1.vReadCheckFile(10, FILE_NONMODIFIED);
    sdFileE2.vReadCheckFile(100, FILE_NONMODIFIED); 
    sdFileE3.vReadCheckFile(1000, FILE_NONMODIFIED); 
    sdFileE4.vReadCheckFile(10000, FILE_NONMODIFIED);
    
    if (!LOG_DETAIL)
    {           
        memset(msg, 0, sizeof(msg));
        sprintf(msg, "Pass: %d", nPass);
        TLIB_LOG(msg);
        memset(msg, 0, sizeof(msg));
        sprintf(msg, "Fail: %d", nFail);
        TLIB_LOG(msg);
        TLIB_ASSERT(nFail == 0);
    }
    TLIB_SUITE_END("Testing LSEEK with SEEK_END");
}

/** \fn void vWaitKeyPress(void)
 *  \brief Waits for the user to press a key.
 */
void vWaitKeyPress(void)
{ 
    printf("\n>PRESS ANY KEY...\n");
    while(1)
    {
        if ( wait_event() & EVT_KBD )
        {
            break;
        }
    }
}    
