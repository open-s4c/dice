#include <stddef.h>
#include <string.h>

#include <dice/compiler.h>
#include <dice/log.h>
#include <dice/types.h>
#include <vsync/spinlock/caslock.h>

static caslock_t registry_lock_;

struct name {
    const char *fallback;
    const char *assigned;
};

#define NAME(ID, ASSIGN)                                                       \
    [ID] = (struct name)                                                       \
    {                                                                          \
        .fallback = #ID, .assigned = ASSIGN,                                   \
    }

#define CHAIN0 "CHAIN_CONTROL"
static struct name chain_names_[] = {
    NAME(0, CHAIN0), NAME(1, NULL),  NAME(2, NULL),  NAME(3, NULL),
    NAME(4, NULL),   NAME(5, NULL),  NAME(6, NULL),  NAME(7, NULL),
    NAME(8, NULL),   NAME(9, NULL),  NAME(10, NULL), NAME(11, NULL),
    NAME(12, NULL),  NAME(13, NULL), NAME(14, NULL), NAME(15, NULL),
};

_Static_assert(sizeof(chain_names_) / sizeof(struct name) == MAX_CHAINS,
               "incorrect number of chains");

#define TYPE0 "ANY_EVENT"
static struct name type_names_[] = {
    NAME(0, TYPE0),  NAME(1, NULL),   NAME(2, NULL),   NAME(3, NULL),
    NAME(4, NULL),   NAME(5, NULL),   NAME(6, NULL),   NAME(7, NULL),
    NAME(8, NULL),   NAME(9, NULL),   NAME(10, NULL),  NAME(11, NULL),
    NAME(12, NULL),  NAME(13, NULL),  NAME(14, NULL),  NAME(15, NULL),
    NAME(16, NULL),  NAME(17, NULL),  NAME(18, NULL),  NAME(19, NULL),
    NAME(20, NULL),  NAME(21, NULL),  NAME(22, NULL),  NAME(23, NULL),
    NAME(24, NULL),  NAME(25, NULL),  NAME(26, NULL),  NAME(27, NULL),
    NAME(28, NULL),  NAME(29, NULL),  NAME(30, NULL),  NAME(31, NULL),
    NAME(32, NULL),  NAME(33, NULL),  NAME(34, NULL),  NAME(35, NULL),
    NAME(36, NULL),  NAME(37, NULL),  NAME(38, NULL),  NAME(39, NULL),
    NAME(40, NULL),  NAME(41, NULL),  NAME(42, NULL),  NAME(43, NULL),
    NAME(44, NULL),  NAME(45, NULL),  NAME(46, NULL),  NAME(47, NULL),
    NAME(48, NULL),  NAME(49, NULL),  NAME(50, NULL),  NAME(51, NULL),
    NAME(52, NULL),  NAME(53, NULL),  NAME(54, NULL),  NAME(55, NULL),
    NAME(56, NULL),  NAME(57, NULL),  NAME(58, NULL),  NAME(59, NULL),
    NAME(60, NULL),  NAME(61, NULL),  NAME(62, NULL),  NAME(63, NULL),
    NAME(64, NULL),  NAME(65, NULL),  NAME(66, NULL),  NAME(67, NULL),
    NAME(68, NULL),  NAME(69, NULL),  NAME(70, NULL),  NAME(71, NULL),
    NAME(72, NULL),  NAME(73, NULL),  NAME(74, NULL),  NAME(75, NULL),
    NAME(76, NULL),  NAME(77, NULL),  NAME(78, NULL),  NAME(79, NULL),
    NAME(80, NULL),  NAME(81, NULL),  NAME(82, NULL),  NAME(83, NULL),
    NAME(84, NULL),  NAME(85, NULL),  NAME(86, NULL),  NAME(87, NULL),
    NAME(88, NULL),  NAME(89, NULL),  NAME(90, NULL),  NAME(91, NULL),
    NAME(92, NULL),  NAME(93, NULL),  NAME(94, NULL),  NAME(95, NULL),
    NAME(96, NULL),  NAME(97, NULL),  NAME(98, NULL),  NAME(99, NULL),
    NAME(100, NULL), NAME(101, NULL), NAME(102, NULL), NAME(103, NULL),
    NAME(104, NULL), NAME(105, NULL), NAME(106, NULL), NAME(107, NULL),
    NAME(108, NULL), NAME(109, NULL), NAME(110, NULL), NAME(111, NULL),
    NAME(112, NULL), NAME(113, NULL), NAME(114, NULL), NAME(115, NULL),
    NAME(116, NULL), NAME(117, NULL), NAME(118, NULL), NAME(119, NULL),
    NAME(120, NULL), NAME(121, NULL), NAME(122, NULL), NAME(123, NULL),
    NAME(124, NULL), NAME(125, NULL), NAME(126, NULL), NAME(127, NULL),
    NAME(128, NULL), NAME(129, NULL), NAME(130, NULL), NAME(131, NULL),
    NAME(132, NULL), NAME(133, NULL), NAME(134, NULL), NAME(135, NULL),
    NAME(136, NULL), NAME(137, NULL), NAME(138, NULL), NAME(139, NULL),
    NAME(140, NULL), NAME(141, NULL), NAME(142, NULL), NAME(143, NULL),
    NAME(144, NULL), NAME(145, NULL), NAME(146, NULL), NAME(147, NULL),
    NAME(148, NULL), NAME(149, NULL), NAME(150, NULL), NAME(151, NULL),
    NAME(152, NULL), NAME(153, NULL), NAME(154, NULL), NAME(155, NULL),
    NAME(156, NULL), NAME(157, NULL), NAME(158, NULL), NAME(159, NULL),
    NAME(160, NULL), NAME(161, NULL), NAME(162, NULL), NAME(163, NULL),
    NAME(164, NULL), NAME(165, NULL), NAME(166, NULL), NAME(167, NULL),
    NAME(168, NULL), NAME(169, NULL), NAME(170, NULL), NAME(171, NULL),
    NAME(172, NULL), NAME(173, NULL), NAME(174, NULL), NAME(175, NULL),
    NAME(176, NULL), NAME(177, NULL), NAME(178, NULL), NAME(179, NULL),
    NAME(180, NULL), NAME(181, NULL), NAME(182, NULL), NAME(183, NULL),
    NAME(184, NULL), NAME(185, NULL), NAME(186, NULL), NAME(187, NULL),
    NAME(188, NULL), NAME(189, NULL), NAME(190, NULL), NAME(191, NULL),
    NAME(192, NULL), NAME(193, NULL), NAME(194, NULL), NAME(195, NULL),
    NAME(196, NULL), NAME(197, NULL), NAME(198, NULL), NAME(199, NULL),
    NAME(200, NULL), NAME(201, NULL), NAME(202, NULL), NAME(203, NULL),
    NAME(204, NULL), NAME(205, NULL), NAME(206, NULL), NAME(207, NULL),
    NAME(208, NULL), NAME(209, NULL), NAME(210, NULL), NAME(211, NULL),
    NAME(212, NULL), NAME(213, NULL), NAME(214, NULL), NAME(215, NULL),
    NAME(216, NULL), NAME(217, NULL), NAME(218, NULL), NAME(219, NULL),
    NAME(220, NULL), NAME(221, NULL), NAME(222, NULL), NAME(223, NULL),
    NAME(224, NULL), NAME(225, NULL), NAME(226, NULL), NAME(227, NULL),
    NAME(228, NULL), NAME(229, NULL), NAME(230, NULL), NAME(231, NULL),
    NAME(232, NULL), NAME(233, NULL), NAME(234, NULL), NAME(235, NULL),
    NAME(236, NULL), NAME(237, NULL), NAME(238, NULL), NAME(239, NULL),
    NAME(240, NULL), NAME(241, NULL), NAME(242, NULL), NAME(243, NULL),
    NAME(244, NULL), NAME(245, NULL), NAME(246, NULL), NAME(247, NULL),
    NAME(248, NULL), NAME(249, NULL), NAME(250, NULL), NAME(251, NULL),
    NAME(252, NULL), NAME(253, NULL), NAME(254, NULL), NAME(255, NULL),
};

_Static_assert(sizeof(type_names_) / sizeof(struct name) == MAX_TYPES,
               "incorrect number of event types");

DICE_HIDE void
ps_registry_add_(bool chain, uint16_t id, const char *name)
{
    uint16_t max       = chain ? MAX_CHAINS : MAX_TYPES;
    const char *what   = chain ? "chain" : "event type";
    struct name *names = chain ? chain_names_ : type_names_;

    caslock_acquire(&registry_lock_);
    if (id >= max)
        log_fatal("cannot add %s %u (max = %u)", what, id, (max - 1));
    if (names[id].assigned == NULL) {
        names[id].assigned = name;
    } else if (strcmp(names[id].assigned, name) == 0) {
        log_debug("adding %s %u again", what, id);
    } else {
        log_fatal("cannot add %s %u again: %s != %s", what, id,
                  names[id].assigned, name);
    }
    caslock_release(&registry_lock_);
}


DICE_HIDE const char *
ps_registry_get_(bool chain, uint16_t id)
{
    uint16_t max       = chain ? MAX_CHAINS : MAX_TYPES;
    const char *what   = chain ? "chain" : "event type";
    struct name *names = chain ? chain_names_ : type_names_;

    caslock_acquire(&registry_lock_);
    if (id >= max)
        log_fatal("cannot find %s %u (max = %u)", what, id, (max - 1));
    const char *name =
        names[id].assigned ? names[id].assigned : names[id].fallback;
    caslock_release(&registry_lock_);
    return name;
}

DICE_HIDE uint16_t
ps_registry_lookup_(bool chain, const char *name)
{
    uint16_t max       = chain ? MAX_CHAINS : MAX_TYPES;
    struct name *names = chain ? chain_names_ : type_names_;
    uint16_t id        = 0;

    caslock_acquire(&registry_lock_);
    for (; id < max; id++) {
        if (names[id].assigned == NULL)
            continue;
        if (strcmp(names[id].assigned, name) == 0)
            break;
    }
    caslock_release(&registry_lock_);
    return id;
}

// -----------------------------------------------------------------------------
// external interfaces
// -----------------------------------------------------------------------------

DICE_WEAK void
ps_register_chain(chain_id id, const char *name)
{
    ps_registry_add_(true, id, name);
}

DICE_WEAK void
ps_register_type(type_id id, const char *name)
{
    return ps_registry_add_(false, id, name);
}

DICE_WEAK const char *
ps_chain_str(chain_id id)
{
    return ps_registry_get_(true, id);
}
DICE_WEAK const char *
ps_type_str(type_id id)
{
    return ps_registry_get_(false, id);
}
DICE_WEAK chain_id
ps_chain_lookup(const char *name)
{
    return ps_registry_lookup_(true, name);
}
DICE_WEAK type_id
ps_type_lookup(const char *name)
{
    return ps_registry_lookup_(false, name);
}
