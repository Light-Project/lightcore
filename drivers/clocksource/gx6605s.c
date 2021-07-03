
#include <asm/io.h>
#include <driver/clocksource.h>
#include <driver/clocksource/timer-gx6605s.h>

static irqreturn_t gx6605s_timer_interrupt(int irq, void *dev)
{
    struct clock_event_device *ce = dev;
    void __iomem *base = timer_of_base(to_timer_of(ce));

    writel(base + TIMER_STATUS, TIMER_STATUS_CLR);
    writel(base + TIMER_INI, 0x00);

    ce->event_handler(ce);

    return IRQ_HANDLED;
}

static int gx6605s_timer_set_oneshot(struct clock_event_device *ce)
{
    void __iomem *base = timer_of_base(to_timer_of(ce));

    /* reset and stop counter */
    writel(base + TIMER_CONTRL, TIMER_CONTRL_RST);

    /* enable with irq and start */
    writel(base + TIMER_CONFIG, TIMER_CONFIG_EN | TIMER_CONFIG_IRQ_EN);

    return 0;
}

static int gx6605s_timer_set_next_event(unsigned long delta, struct clock_event_device *ce)
{
    void __iomem *base = timer_of_base(to_timer_of(ce));

    /* use reset to pause timer */
    writel(base + TIMER_CONTRL, TIMER_CONTRL_RST);

    /* config next timeout value */
    writel(ULONG_MAX - delta, base + TIMER_INI);
    writel(TIMER_CONTRL_START, base + TIMER_CONTRL);

    return 0;
}

static int gx6605s_timer_shutdown(struct clock_event_device *ce)
{
    void __iomem *base = timer_of_base(to_timer_of(ce));

    /* shutdown the timer */
    writel(base + TIMER_CONTRL, 0x00);
    writel(base + TIMER_CONFIG, 0x00);

    return 0;
}
TIMER_DT_DECLARE(csky_gx6605s_timer, "csky,gx6605s-timer", gx6605s_timer_set_next_event);
