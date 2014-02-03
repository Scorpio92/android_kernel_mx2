/*
 * ALSA SoC SPDIF DIT driver
 *
 *  This driver is used by controllers which can operate in DIT (SPDI/F) where
 *  no codec is needed.  This file provides stub codec that can be used
 *  in these configurations. TI DaVinci Audio controller uses this driver.
 *
 * Author:      Steve Chen,  <schen@mvista.com>
 * Copyright:   (C) 2009 MontaVista Software, Inc., <source@mvista.com>
 * Copyright:   (C) 2009  Texas Instruments, India
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <sound/soc.h>
#include <sound/pcm.h>
#include <sound/initval.h>

#define DRV_NAME "spdif-dit"

MODULE_LICENSE("GPL");

#define STUB_RATES	SNDRV_PCM_RATE_8000_96000
//#define STUB_FORMATS	SNDRV_PCM_FMTBIT_S16_LE
#define STUB_FORMATS (SNDRV_PCM_FMTBIT_S16 |	    SNDRV_PCM_FMTBIT_U16 |\
				    SNDRV_PCM_FMTBIT_U8 |\
				    SNDRV_PCM_FMTBIT_S8 |\
				    SNDRV_PCM_FMTBIT_IEC958_SUBFRAME |\
				    SNDRV_PCM_FMTBIT_S24 |\
				    SNDRV_PCM_FMTBIT_U24)
#if defined(CONFIG_MX_SERIAL_TYPE) || defined(CONFIG_MX2_SERIAL_TYPE)
extern void audio_spdif_output_en(int on);
#endif
static int spdif_set_bias_level(struct snd_soc_codec *codec,
				 enum snd_soc_bias_level level)
{
    pr_debug("%s:%s => %s\r\n", __func__,(codec->dapm.bias_level == SND_SOC_BIAS_ON?"On"
		:codec->dapm.bias_level == SND_SOC_BIAS_OFF?"Off"
		:codec->dapm.bias_level == SND_SOC_BIAS_PREPARE?"Prepare"
		:codec->dapm.bias_level == SND_SOC_BIAS_STANDBY?"Standby":"Unknown"),
		(level == SND_SOC_BIAS_ON?"On"
		:level == SND_SOC_BIAS_OFF?"Off"
		:level == SND_SOC_BIAS_PREPARE?"Prepare"
		:level == SND_SOC_BIAS_STANDBY?"Standby":"Unknown"));

	switch (level) {
	case SND_SOC_BIAS_ON:
#if defined(CONFIG_MX_SERIAL_TYPE) || defined(CONFIG_MX2_SERIAL_TYPE)
	audio_spdif_output_en(1);
#endif
		break;
	case SND_SOC_BIAS_PREPARE:
	case SND_SOC_BIAS_STANDBY:
#if defined(CONFIG_MX_SERIAL_TYPE) || defined(CONFIG_MX2_SERIAL_TYPE)
	audio_spdif_output_en(0);
#endif
		break;
	case SND_SOC_BIAS_OFF:
#if defined(CONFIG_MX_SERIAL_TYPE) || defined(CONFIG_MX2_SERIAL_TYPE)
	audio_spdif_output_en(0);
#endif
		break;
	}
	codec->dapm.bias_level = level;
	return 0;
}
static struct snd_soc_codec_driver soc_codec_spdif_dit = {
	.set_bias_level = spdif_set_bias_level,
};

static struct snd_soc_dai_driver dit_stub_dai = {
	.name		= "dit-hifi",
	.playback 	= {
		.stream_name	= "Playback",
		.channels_min	= 1,
		.channels_max	= 384,
		.rates		= STUB_RATES,
		.formats	= STUB_FORMATS,
	},
};

static int __devinit spdif_dit_probe(struct platform_device *pdev)
{
	return snd_soc_register_codec(&pdev->dev, &soc_codec_spdif_dit,
			&dit_stub_dai, 1);
}

static int spdif_dit_remove(struct platform_device *pdev)
{
	snd_soc_unregister_codec(&pdev->dev);
	return 0;
}

static struct platform_driver spdif_dit_driver = {
	.probe		= spdif_dit_probe,
	.remove		= __devexit_p(spdif_dit_remove),
	.driver		= {
		.name	= DRV_NAME,
		.owner	= THIS_MODULE,
	},
};

static int __init dit_modinit(void)
{
	return platform_driver_register(&spdif_dit_driver);
}

static void __exit dit_exit(void)
{
	platform_driver_unregister(&spdif_dit_driver);
}

module_init(dit_modinit);
module_exit(dit_exit);

MODULE_AUTHOR("Steve Chen <schen@mvista.com>");
MODULE_DESCRIPTION("SPDIF dummy codec driver");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:" DRV_NAME);
