//
// Created by user on 3/25/18.
//

#ifdef _WIN32
#include <sys/time.h>
int c_gettimeofday(struct timeval *tv, struct timezone *tz) {
  return gettimeofday(tv, tz);
}
#endif


#include <xsAll.h>
#include <xs/xsScript.h>
#include <xs/xsCommon.h>
#include "download.h"
#include "string.h"

void fxCreateMachinePlatform(txMachine* the) {
}

void fxDeleteMachinePlatform(txMachine* the) {
}

void fxBuildKeys(txMachine* the) {
    int i;
    for (i = 0; i < XS_SYMBOL_ID_COUNT; i++) {
        txID id = the->keyIndex;
        txSlot* description = fxNewSlot(the);
        fxCopyStringC(the, description, gxIDStrings[i]);
        the->keyArray[id] = description;
        the->keyIndex++;
    }
    for (; i < XS_ID_COUNT; i++) {
        fxID((txMachine *) the, gxIDStrings[i]);
    }
}

void* fxAllocateChunks(txMachine* the, txSize theSize) {
    return malloc((size_t) theSize);
}

void fxFreeChunks(txMachine* the, void* theChunks) {
    free(theChunks);
}

txSlot* fxAllocateSlots(txMachine* the, txSize theCount) {
    return(txSlot*)malloc(theCount * sizeof(txSlot));
}

void fxFreeSlots(txMachine* the, void* theSlots) {
    free(theSlots);
}

void fxMarkHost(txMachine* the, txMarkRoot markRoot) {
}

void fxSweepHost(txMachine* the) {
}



txID fxFindModule(txMachine* the, txID moduleID, txSlot* slot) {
    char *name = fxToString(the, slot);

    hinarin_string *path = hinarin_string_new();
    hinarin_string_append(path, name);

    bool exists = false;

    if (!hinarin_string_contains(path, "://")) {
        hinarin_string_insert(path, 0, "file://");
        hinarin_string_append(path, ".js");

        hinarin_download_free_result(hinarin_download_to_exists(hinarin_download_request_new(path->data, NULL, NULL), &exists));
        if (!exists) {
            hinarin_string_insert(path, path->length-3, "/index");
            hinarin_download_free_result(hinarin_download_to_exists(hinarin_download_request_new(path->data, NULL, NULL), &exists));
        }

        if (!exists) {
            path->data[0] = '\0';
            path->length = 0;
            hinarin_string_append(path, "https://raw.githubusercontent.com/");
            hinarin_string_append(path, name);
            hinarin_string_append(path, ".js");

            hinarin_download_free_result(hinarin_download_to_exists(hinarin_download_request_new(path->data, NULL, NULL), &exists));
            if (!exists) {
                hinarin_string_insert(path, path->length-3, "/index");
                hinarin_download_free_result(hinarin_download_to_exists(hinarin_download_request_new(path->data, NULL, NULL), &exists));
            }
        }
    } else {
        hinarin_download_free_result(hinarin_download_to_exists(hinarin_download_request_new(path->data, NULL, NULL), &exists));
        if (!exists) {
            hinarin_string_append(path, ".js");
            hinarin_download_free_result(hinarin_download_to_exists(hinarin_download_request_new(path->data, NULL, NULL), &exists));
        }
    }

    if (!exists) {
        hinarin_string_free(path);
        return XS_NO_ID;
    }

    txID id = fxID(the, path->data);
    hinarin_string_free(path);
    return id;
}

void fxLoadModule(txMachine* the, txID moduleID) {
    char *name = fxName(the, moduleID);
    printf("Loading %s\n", name);
    hinarin_string *string = hinarin_string_new();
    hinarin_download_free_result(hinarin_download_to_string(hinarin_download_request_new(name, NULL, NULL), string));

    txStringCStream stream;
    stream.buffer = string->data;
    stream.offset = 0;
    stream.size = (txSize) string->length;

    txScript *script = fxParseScript(the, &stream, fxStringCGetter, mxStrictFlag);
    hinarin_string_free(string);

    fxResolveModule(the, moduleID, script, NULL, NULL);
}

txScript* fxParseScript(txMachine* the, void* stream, txGetter getter, txUnsigned flags) {
    txParser parser;
    txParserJump jump;
    txScript *script = NULL;
    fxInitializeParser(&parser, the, 32*1024, 1993);
    parser.firstJump = &jump;
    if (setjmp(jump.jmp_buf) == 0) {
        fxParserTree(&parser, stream, getter, flags, NULL);
        fxParserHoist(&parser);
        fxParserBind(&parser);
        script = fxParserCode(&parser);
    }
    fxTerminateParser(&parser);
    return script;
}

    void fxRunModule(txMachine* the, txString path) {
        mxPushStringC(path);
        fxRequireModule(the, XS_NO_ID, the->stack);
        the->stack++;
    }

void fxRunProgram(txMachine* the, txString path) {
    hinarin_string *string = hinarin_string_new();
    hinarin_download_free_result(hinarin_download_to_string(hinarin_download_request_new(path, NULL, NULL), string));

    txStringCStream stream;
    stream.buffer = string->data;
    stream.offset = 0;
    stream.size = (txSize) string->length;

    txScript *script = fxParseScript(the, &stream, fxStringCGetter, mxProgramFlag | mxStrictFlag);
    hinarin_string_free(string);

    fxRunScript(the, script, &mxGlobal, C_NULL, mxClosures.value.reference, C_NULL, C_NULL);
    mxPullSlot(mxResult);
}

void fxQueuePromiseJobs(txMachine* the) {
}

void fxAbort(txMachine* the) {
    exit(0);
}

void fxConnect(txMachine* the) {
}

void fxDisconnect(txMachine* the) {
}

txBoolean fxIsConnected(txMachine* the) {
    return 0;
}

txBoolean fxIsReadable(txMachine* the) {
    return 0;
}

void fxReceive(txMachine* the) {
}

void fxSend(txMachine* the, txBoolean more) {
}
