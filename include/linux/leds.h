/*
 * Driver model for leds and led triggers
 *
 * Copyright (C) 2005 John Lenz <lenz@cs.wisc.edu>
 * Copyright (C) 2005 Richard Purdie <rpurdie@openedhand.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */
#ifndef __LINUX_LEDS_H_INCLUDED
#define __LINUX_LEDS_H_INCLUDED

#include <linux/device.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/rwsem.h>
#include <linux/spinlock.h>
#include <linux/timer.h>
#include <linux/workqueue.h>

struct device;
/*
 * LED Core
 */

enum led_brightness {
	LED_OFF		= 0,
	LED_0		= 0,
	LED_1		= 3,
	LED_2		= 6,
	LED_3		= 8,
	LED_4		= 11,
	LED_5		= 14,
	LED_6		= 16,
	LED_7		= 19,
	LED_8		= 21,
	LED_9		= 24,
	LED_10		= 26,
	LED_11		= 29,
	LED_12		= 31,
	LED_13		= 34,
	LED_14		= 36,
	LED_15		= 39,
	LED_16		= 42,
	LED_17		= 44,
	LED_18		= 47,
	LED_19		= 49,
	LED_20		= 51,
	LED_21		= 54,
	LED_22		= 57,
	LED_23		= 59,
	LED_24		= 62,
	LED_25		= 65,
	LED_26		= 67,
	LED_27		= 70,
	LED_28		= 72,
	LED_29		= 75,
	LED_30		= 77,
	LED_31		= 80,
	LED_32		= 82,
	LED_33		= 85,
	LED_34		= 87,
	LED_35		= 90,
	LED_36		= 93,
	LED_37		= 95,
	LED_38		= 98,
	LED_39		= 100,
	LED_40		= 103,
	LED_41		= 105,
	LED_42		= 108,
	LED_43		= 110,
	LED_44		= 113,
	LED_45		= 116,
	LED_46		= 118,
	LED_47		= 121,
	LED_48		= 123,
	LED_49		= 125,
	LED_50		= 127,
	LED_HALF	= 127,
	LED_51		= 131,
	LED_52		= 133,
	LED_53		= 136,
	LED_54		= 138,
	LED_55		= 141,
	LED_56		= 144,
	LED_57		= 146,
	LED_58		= 149,
	LED_59		= 151,
	LED_60		= 154,
	LED_61		= 156,
	LED_62		= 159,
	LED_63		= 161,
	LED_64		= 164,
	LED_65		= 167,
	LED_66		= 169,
	LED_67		= 172,
	LED_68		= 174,
	LED_69		= 177,
	LED_70		= 179,
	LED_71		= 182,
	LED_72		= 184,
	LED_73		= 187,
	LED_74		= 189,
	LED_75		= 192,
	LED_76		= 195,
	LED_77		= 197,
	LED_78		= 200,
	LED_79		= 202,
	LED_80		= 205,
	LED_81		= 207,
	LED_82		= 210,
	LED_83		= 212,
	LED_84		= 215,
	LED_85		= 218,
	LED_86		= 220,
	LED_87		= 223,
	LED_88		= 225,
	LED_89		= 228,
	LED_90		= 230,
	LED_91		= 233,
	LED_92		= 235,
	LED_93		= 238,
	LED_94		= 240,
	LED_95		= 243,
	LED_96		= 246,
	LED_97		= 248,
	LED_98		= 251,
	LED_99		= 253,
	LED_100		= 255,
	LED_FULL	= 255,
};

struct led_classdev {
	const char		*name;
	enum led_brightness	 brightness;
	enum led_brightness	 max_brightness;
	enum led_brightness	 usr_brightness_req;
	int			 flags;

	/* Lower 16 bits reflect status */
#define LED_SUSPENDED		BIT(0)
	/* Upper 16 bits reflect control information */
#define LED_CORE_SUSPENDRESUME	BIT(16)
#define LED_BLINK_ONESHOT	BIT(17)
#define LED_BLINK_ONESHOT_STOP	BIT(18)
#define LED_BLINK_INVERT	BIT(19)
#define LED_SYSFS_DISABLE	BIT(20)
#define SET_BRIGHTNESS_ASYNC	BIT(21)
#define SET_BRIGHTNESS_SYNC	BIT(22)
#define LED_DEV_CAP_FLASH	BIT(23)
#define LED_KEEP_TRIGGER	BIT(24)
#define LED_BRIGHTNESS_FAST	BIT(25)

	/* Set LED brightness level */
	/* Must not sleep, use a workqueue if needed */
	void		(*brightness_set)(struct led_classdev *led_cdev,
					  enum led_brightness brightness);
	/*
	 * Set LED brightness level immediately - it can block the caller for
	 * the time required for accessing a LED device register.
	 */
	int		(*brightness_set_sync)(struct led_classdev *led_cdev,
					enum led_brightness brightness);
	/* Get LED brightness level */
	enum led_brightness (*brightness_get)(struct led_classdev *led_cdev);

	/*
	 * Activate hardware accelerated blink, delays are in milliseconds
	 * and if both are zero then a sensible default should be chosen.
	 * The call should adjust the timings in that case and if it can't
	 * match the values specified exactly.
	 * Deactivate blinking again when the brightness is set to a fixed
	 * value via the brightness_set() callback.
	 */
	int		(*blink_set)(struct led_classdev *led_cdev,
				     unsigned long *delay_on,
				     unsigned long *delay_off);

	struct device		*dev;
	const struct attribute_group	**groups;

	struct list_head	 node;			/* LED Device list */
	const char		*default_trigger;	/* Trigger to use */

	unsigned long		 blink_delay_on, blink_delay_off;
	struct timer_list	 blink_timer;
	int			 blink_brightness;
	void			(*flash_resume)(struct led_classdev *led_cdev);

	struct work_struct	set_brightness_work;
	int			delayed_set_value;

#ifdef CONFIG_LEDS_TRIGGERS
	/* Protects the trigger data below */
	struct rw_semaphore	 trigger_lock;

	struct led_trigger	*trigger;
	struct list_head	 trig_list;
	void			*trigger_data;
	/* true if activated - deactivate routine uses it to do cleanup */
	bool			activated;
#endif

	/* Ensures consistent access to the LED Flash Class device */
	struct mutex		led_access;
};

extern int led_classdev_register(struct device *parent,
				 struct led_classdev *led_cdev);
extern int devm_led_classdev_register(struct device *parent,
				      struct led_classdev *led_cdev);
extern void led_classdev_unregister(struct led_classdev *led_cdev);
extern void devm_led_classdev_unregister(struct device *parent,
					 struct led_classdev *led_cdev);
extern void led_classdev_suspend(struct led_classdev *led_cdev);
extern void led_classdev_resume(struct led_classdev *led_cdev);

/**
 * led_blink_set - set blinking with software fallback
 * @led_cdev: the LED to start blinking
 * @delay_on: the time it should be on (in ms)
 * @delay_off: the time it should ble off (in ms)
 *
 * This function makes the LED blink, attempting to use the
 * hardware acceleration if possible, but falling back to
 * software blinking if there is no hardware blinking or if
 * the LED refuses the passed values.
 *
 * Note that if software blinking is active, simply calling
 * led_cdev->brightness_set() will not stop the blinking,
 * use led_classdev_brightness_set() instead.
 */
extern void led_blink_set(struct led_classdev *led_cdev,
			  unsigned long *delay_on,
			  unsigned long *delay_off);
/**
 * led_blink_set_oneshot - do a oneshot software blink
 * @led_cdev: the LED to start blinking
 * @delay_on: the time it should be on (in ms)
 * @delay_off: the time it should ble off (in ms)
 * @invert: blink off, then on, leaving the led on
 *
 * This function makes the LED blink one time for delay_on +
 * delay_off time, ignoring the request if another one-shot
 * blink is already in progress.
 *
 * If invert is set, led blinks for delay_off first, then for
 * delay_on and leave the led on after the on-off cycle.
 */
extern void led_blink_set_oneshot(struct led_classdev *led_cdev,
				  unsigned long *delay_on,
				  unsigned long *delay_off,
				  int invert);
/**
 * led_set_brightness - set LED brightness
 * @led_cdev: the LED to set
 * @brightness: the brightness to set it to
 *
 * Set an LED's brightness, and, if necessary, cancel the
 * software blink timer that implements blinking when the
 * hardware doesn't.
 */
extern void led_set_brightness(struct led_classdev *led_cdev,
			       enum led_brightness brightness);
/**
 * led_update_brightness - update LED brightness
 * @led_cdev: the LED to query
 *
 * Get an LED's current brightness and update led_cdev->brightness
 * member with the obtained value.
 *
 * Returns: 0 on success or negative error value on failure
 */
extern int led_update_brightness(struct led_classdev *led_cdev);

/**
 * led_sysfs_disable - disable LED sysfs interface
 * @led_cdev: the LED to set
 *
 * Disable the led_cdev's sysfs interface.
 */
extern void led_sysfs_disable(struct led_classdev *led_cdev);

/**
 * led_sysfs_enable - enable LED sysfs interface
 * @led_cdev: the LED to set
 *
 * Enable the led_cdev's sysfs interface.
 */
extern void led_sysfs_enable(struct led_classdev *led_cdev);

/**
 * led_sysfs_is_disabled - check if LED sysfs interface is disabled
 * @led_cdev: the LED to query
 *
 * Returns: true if the led_cdev's sysfs interface is disabled.
 */
static inline bool led_sysfs_is_disabled(struct led_classdev *led_cdev)
{
	return led_cdev->flags & LED_SYSFS_DISABLE;
}

/*
 * LED Triggers
 */
/* Registration functions for simple triggers */
#define DEFINE_LED_TRIGGER(x)		static struct led_trigger *x;
#define DEFINE_LED_TRIGGER_GLOBAL(x)	struct led_trigger *x;

#ifdef CONFIG_LEDS_TRIGGERS

#define TRIG_NAME_MAX 50

struct led_trigger {
	/* Trigger Properties */
	const char	 *name;
	void		(*activate)(struct led_classdev *led_cdev);
	void		(*deactivate)(struct led_classdev *led_cdev);

	/* LEDs under control by this trigger (for simple triggers) */
	rwlock_t	  leddev_list_lock;
	struct list_head  led_cdevs;

	/* Link to next registered trigger */
	struct list_head  next_trig;
};

ssize_t led_trigger_store(struct device *dev, struct device_attribute *attr,
			const char *buf, size_t count);
ssize_t led_trigger_show(struct device *dev, struct device_attribute *attr,
			char *buf);

/* Registration functions for complex triggers */
extern int led_trigger_register(struct led_trigger *trigger);
extern void led_trigger_unregister(struct led_trigger *trigger);

extern void led_trigger_register_simple(const char *name,
				struct led_trigger **trigger);
extern void led_trigger_unregister_simple(struct led_trigger *trigger);
extern void led_trigger_event(struct led_trigger *trigger,
				enum led_brightness event);
extern void led_trigger_blink(struct led_trigger *trigger,
			      unsigned long *delay_on,
			      unsigned long *delay_off);
extern void led_trigger_blink_oneshot(struct led_trigger *trigger,
				      unsigned long *delay_on,
				      unsigned long *delay_off,
				      int invert);
extern void led_trigger_set_default(struct led_classdev *led_cdev);
extern void led_trigger_set(struct led_classdev *led_cdev,
			struct led_trigger *trigger);
extern void led_trigger_remove(struct led_classdev *led_cdev);

static inline void *led_get_trigger_data(struct led_classdev *led_cdev)
{
	return led_cdev->trigger_data;
}

/**
 * led_trigger_rename_static - rename a trigger
 * @name: the new trigger name
 * @trig: the LED trigger to rename
 *
 * Change a LED trigger name by copying the string passed in
 * name into current trigger name, which MUST be large
 * enough for the new string.
 *
 * Note that name must NOT point to the same string used
 * during LED registration, as that could lead to races.
 *
 * This is meant to be used on triggers with statically
 * allocated name.
 */
extern void led_trigger_rename_static(const char *name,
				      struct led_trigger *trig);

#else

/* Trigger has no members */
struct led_trigger {};

/* Trigger inline empty functions */
static inline void led_trigger_register_simple(const char *name,
					struct led_trigger **trigger) {}
static inline void led_trigger_unregister_simple(struct led_trigger *trigger) {}
static inline void led_trigger_event(struct led_trigger *trigger,
				enum led_brightness event) {}
static inline void led_trigger_blink(struct led_trigger *trigger,
				      unsigned long *delay_on,
				      unsigned long *delay_off) {}
static inline void led_trigger_blink_oneshot(struct led_trigger *trigger,
				      unsigned long *delay_on,
				      unsigned long *delay_off,
				      int invert) {}
static inline void led_trigger_set_default(struct led_classdev *led_cdev) {}
static inline void led_trigger_set(struct led_classdev *led_cdev,
				struct led_trigger *trigger) {}
static inline void led_trigger_remove(struct led_classdev *led_cdev) {}
static inline void *led_get_trigger_data(struct led_classdev *led_cdev)
{
	return NULL;
}

#endif /* CONFIG_LEDS_TRIGGERS */

/* Trigger specific functions */
#ifdef CONFIG_LEDS_TRIGGER_IDE_DISK
extern void ledtrig_ide_activity(void);
#else
static inline void ledtrig_ide_activity(void) {}
#endif

#if defined(CONFIG_LEDS_TRIGGER_CAMERA) || defined(CONFIG_LEDS_TRIGGER_CAMERA_MODULE)
extern void ledtrig_flash_ctrl(bool on);
extern void ledtrig_torch_ctrl(bool on);
#else
static inline void ledtrig_flash_ctrl(bool on) {}
static inline void ledtrig_torch_ctrl(bool on) {}
#endif

/*
 * Generic LED platform data for describing LED names and default triggers.
 */
struct led_info {
	const char	*name;
	const char	*default_trigger;
	int		flags;
};

struct led_platform_data {
	int		num_leds;
	struct led_info	*leds;
};

/* For the leds-gpio driver */
struct gpio_led {
	const char *name;
	const char *default_trigger;
	unsigned 	gpio;
	unsigned	active_low : 1;
	unsigned	retain_state_suspended : 1;
	unsigned	default_state : 2;
	/* default_state should be one of LEDS_GPIO_DEFSTATE_(ON|OFF|KEEP) */
	struct gpio_desc *gpiod;
};
#define LEDS_GPIO_DEFSTATE_OFF		0
#define LEDS_GPIO_DEFSTATE_ON		1
#define LEDS_GPIO_DEFSTATE_KEEP		2

struct gpio_led_platform_data {
	int 		num_leds;
	const struct gpio_led *leds;

#define GPIO_LED_NO_BLINK_LOW	0	/* No blink GPIO state low */
#define GPIO_LED_NO_BLINK_HIGH	1	/* No blink GPIO state high */
#define GPIO_LED_BLINK		2	/* Please, blink */
	int		(*gpio_blink_set)(struct gpio_desc *desc, int state,
					unsigned long *delay_on,
					unsigned long *delay_off);
};

struct platform_device *gpio_led_register_device(
		int id, const struct gpio_led_platform_data *pdata);

enum cpu_led_event {
	CPU_LED_IDLE_START,	/* CPU enters idle */
	CPU_LED_IDLE_END,	/* CPU idle ends */
	CPU_LED_START,		/* Machine starts, especially resume */
	CPU_LED_STOP,		/* Machine stops, especially suspend */
	CPU_LED_HALTED,		/* Machine shutdown */
};
#ifdef CONFIG_LEDS_TRIGGER_CPU
extern void ledtrig_cpu(enum cpu_led_event evt);
#else
static inline void ledtrig_cpu(enum cpu_led_event evt)
{
	return;
}
#endif

#endif		/* __LINUX_LEDS_H_INCLUDED */
