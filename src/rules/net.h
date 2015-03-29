
#include "rete.h"
#ifdef _WIN32
#include "../../deps/hiredis_win/hiredis.h"
#include "../../deps/hiredis_win/sds.h"
#else
#include "../../deps/hiredis/hiredis.h"
#include "../../deps/hiredis/sds.h"
#endif

#define HASH_LENGTH 40
#define MAX_MESSAGE_BATCH 64

typedef char functionHash[HASH_LENGTH + 1];

typedef struct binding {
    redisContext *reContext;
    functionHash addMessageHash;
    functionHash peekActionHash;
    functionHash removeActionHash;
    functionHash partitionHash;
    functionHash timersHash;
    char *sessionHashset;
    char *factsHashset;
    char *eventsHashset;
    char *timersSortedset;
    functionHash *hashArray;
    unsigned int hashArrayLength;
} binding;

typedef struct bindingsList {
    binding *bindings;
    unsigned int bindingsLength;
    unsigned int lastBinding;
    unsigned int lastTimersBinding;
} bindingsList;

unsigned int getBindingIndex(ruleset *tree, 
                             unsigned int sidHash, 
                             unsigned int *bindingIndex);

unsigned int formatEvalMessage(void *rulesBinding, 
                               char *key, 
                               char *sid, 
                               char *mid, 
                               char *message, 
                               jsonProperty *allProperties,
                               unsigned int propertiesLength,
                               unsigned int actionIndex,
                               unsigned char assertFact,
                               char **command);

unsigned int formatStoreMessage(void *rulesBinding, 
                                char *key, 
                                char *sid, 
                                char *message, 
                                jsonProperty *allProperties,
                                unsigned int propertiesLength,
                                unsigned char storeFact,
                                char **command);

unsigned int formatStoreSession(void *rulesBinding, 
                                char *sid, 
                                char *state, 
                                unsigned char tryExists,
                                char **command);

unsigned int formatStoreSessionFact(void *rulesBinding, 
                                    char *sid, 
                                    char *message,
                                    unsigned char tryExists, 
                                    char **command);

unsigned int formatRemoveTimer(void *rulesBinding, 
                               char *timer,
                               char **command);

unsigned int formatRemoveAction(void *rulesBinding, 
                                char *sid, 
                                char **command);

unsigned int formatRemoveMessage(void *rulesBinding, 
                                 char *sid, 
                                 char *mid,
                                 unsigned char removeFact,
                                 char **command);

unsigned int executeBatch(void *rulesBinding,
                          char **commands,
                          unsigned short commandCount);

unsigned int startNonBlockingBatch(void *rulesBinding,
                                   char **commands,
                                   unsigned short commandCount,
                                   unsigned short *replyCount); 

unsigned int completeNonBlockingBatch(void *rulesBinding,
                                      unsigned short replyCount);

unsigned int removeMessage(void *rulesBinding, 
                            char *sid, 
                            char *mid);

unsigned int peekAction(ruleset *tree, 
                        void **bindingContext, 
                        redisReply **reply);

unsigned int peekTimers(ruleset *tree, 
                        void **bindingContext, 
                        redisReply **reply);

unsigned int registerTimer(void *rulesBinding, 
                           unsigned int duration, 
                           char *timer);

unsigned int deleteBindingsList(ruleset *tree);

unsigned int getSession(void *rulesBinding, 
                        char *sid, 
                        char **state);


