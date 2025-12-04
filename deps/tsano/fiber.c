void *
__tsan_get_current_fiber(void)
{
    return (void *)0;
}

void *
__tsan_create_fiber(unsigned flags)
{
    (void)flags;
    return (void *)0;
}

void
__tsan_destroy_fiber(void *fiber)
{
    (void)fiber;
}

void
__tsan_switch_to_fiber(void *fiber, unsigned flags)
{
    (void)fiber;
    (void)flags;
}

void
__tsan_set_fiber_name(void *fiber, const char *name)
{
    (void)fiber;
    (void)name;
}
