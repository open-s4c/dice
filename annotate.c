void
AnnotateHappensBefore(char *f, int l, void *addr)
{
    (void)f;
    (void)l;
    (void)addr;
}

void
AnnotateHappensAfter(char *f, int l, void *addr)
{
    (void)f;
    (void)l;
    (void)addr;
}

void
AnnotateCondVarSignal(char *f, int l, void *cv)
{
    (void)f;
    (void)l;
    (void)cv;
}

void
AnnotateCondVarSignalAll(char *f, int l, void *cv)
{
    (void)f;
    (void)l;
    (void)cv;
}

void
AnnotateMutexIsNotPHB(char *f, int l, void *mu)
{
    (void)f;
    (void)l;
    (void)mu;
}

void
AnnotateCondVarWait(char *f, int l, void *cv, void *lock)
{
    (void)f;
    (void)l;
    (void)cv;
    (void)lock;
}

void
AnnotateRWLockCreate(char *f, int l, void *m)
{
    (void)f;
    (void)l;
    (void)m;
}

void
AnnotateRWLockCreateStatic(char *f, int l, void *m)
{
    (void)f;
    (void)l;
    (void)m;
}

void
AnnotateRWLockDestroy(char *f, int l, void *m)
{
    (void)f;
    (void)l;
    (void)m;
}

void
AnnotateRWLockAcquired(char *f, int l, void *m, void *is_w)
{
    (void)f;
    (void)l;
    (void)m;
    (void)is_w;
}

void
AnnotateRWLockReleased(char *f, int l, void *m, void *is_w)
{
    (void)f;
    (void)l;
    (void)m;
    (void)is_w;
}

void
AnnotateTraceMemory(char *f, int l, void *mem)
{
    (void)f;
    (void)l;
    (void)mem;
}

void
AnnotateFlushState(char *f, int l)
{
    (void)f;
    (void)l;
}

void
AnnotateNewMemory(char *f, int l, void *mem, void *size)
{
    (void)f;
    (void)l;
    (void)mem;
    (void)size;
}

void
AnnotateNoOp(char *f, int l, void *mem)
{
    (void)f;
    (void)l;
    (void)mem;
}

void
AnnotateFlushExpectedRaces(char *f, int l)
{
    (void)f;
    (void)l;
}

void
AnnotateEnableRaceDetection(char *f, int l, int enable)
{
    (void)f;
    (void)l;
    (void)enable;
}

void
AnnotateMutexIsUsedAsCondVar(char *f, int l, void *mu)
{
    (void)f;
    (void)l;
    (void)mu;
}

void
AnnotatePCQGet(char *f, int l, void *pcq)
{
    (void)f;
    (void)l;
    (void)pcq;
}

void
AnnotatePCQPut(char *f, int l, void *pcq)
{
    (void)f;
    (void)l;
    (void)pcq;
}

void
AnnotatePCQDestroy(char *f, int l, void *pcq)
{
    (void)f;
    (void)l;
    (void)pcq;
}

void
AnnotatePCQCreate(char *f, int l, void *pcq)
{
    (void)f;
    (void)l;
    (void)pcq;
}

void
AnnotateExpectRace(char *f, int l, void *mem, char *desc)
{
    (void)f;
    (void)l;
    (void)mem;
    (void)desc;
}

void
AnnotateBenignRaceSized(char *f, int l, void *mem, void *size, char *desc)
{
    (void)f;
    (void)l;
    (void)mem;
    (void)size;
    (void)desc;
}

void
AnnotateBenignRace(char *f, int l, void *mem, char *desc)
{
    (void)f;
    (void)l;
    (void)mem;
    (void)desc;
}

void
AnnotateIgnoreReadsBegin(char *f, int l)
{
    (void)f;
    (void)l;
}

void
AnnotateIgnoreReadsEnd(char *f, int l)
{
    (void)f;
    (void)l;
}

void
AnnotateIgnoreWritesBegin(char *f, int l)
{
    (void)f;
    (void)l;
}

void
AnnotateIgnoreWritesEnd(char *f, int l)
{
    (void)f;
    (void)l;
}

void
AnnotateIgnoreSyncBegin(char *f, int l)
{
    (void)f;
    (void)l;
}

void
AnnotateIgnoreSyncEnd(char *f, int l)
{
    (void)f;
    (void)l;
}

void
AnnotatePublishMemoryRange(char *f, int l, void *addr, void *size)
{
    (void)f;
    (void)l;
    (void)addr;
    (void)size;
}

void
AnnotateUnpublishMemoryRange(char *f, int l, void *addr, void *size)
{
    (void)f;
    (void)l;
    (void)addr;
    (void)size;
}

void
AnnotateThreadName(char *f, int l, char *name)
{
    (void)f;
    (void)l;
    (void)name;
}

void
WTFAnnotateHappensBefore(char *f, int l, void *addr)
{
    (void)f;
    (void)l;
    (void)addr;
}

void
WTFAnnotateHappensAfter(char *f, int l, void *addr)
{
    (void)f;
    (void)l;
    (void)addr;
}

void
WTFAnnotateBenignRaceSized(char *f, int l, void *mem, void *sz, char *desc)
{
    (void)f;
    (void)l;
    (void)mem;
    (void)sz;
    (void)desc;
}

void
AnnotateMemoryIsInitialized(char *f, int l, void *mem, void *sz)
{
    (void)f;
    (void)l;
    (void)mem;
    (void)sz;
}

void
AnnotateMemoryIsUninitialized(char *f, int l, void *mem, void *sz)
{
    (void)f;
    (void)l;
    (void)mem;
    (void)sz;
}
