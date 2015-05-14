sinclude .config
#CROSS_COMPILE:=$(HOME)/usr/arm/4.3.3/bin/arm-linux-
#CROSS_COMPILE:=arm-linux-
CROSS_COMPILE:=$(compile)
CC=$(CROSS_COMPILE)g++
STRIP=$(CROSS_COMPILE)strip
TOPDIR=$(PWD)/
MAIN=$(PWD)/main/
FREETYPE=$(TOPDIR)/include/freetype2
CFLAG+=-I$(TOPDIR)
CFLAG+=-I$(MAIN)
CFLAG+=-I$(TOPDIR)include
CFLAG+=-I$(FREETYPE)

#CFLAG+=-g -O1
LDFLAG += -lpthread -lc -lgcc -ldl -rdynamic -lrt -lts
OUTPUT =../output/
TARGET = $(OUTPUT)sparrow
OBJS_DIR = $(TOPDIR)/objs/
DRAGON_AUTO = $(TOPDIR)/script/dragon_auto.sh
MCONF = $(TOPDIR)/script/mconf
CONF = $(TOPDIR)/script/conf
MKZLIB = $(TOPDIR)/script/mk.zlib.sh
MKPNGLIB = $(TOPDIR)/script/mk.libpng.sh
MKICONVLIB = $(TOPDIR)/script/mk.iconv.sh
MKFREETYPELIB = $(TOPDIR)/script/mk.freetype.sh
MKAUTO=Makefile.auto

MAKE=make CROSS_COMPILE=$(CROSS_COMPILE) CC=$(CC) CFLAG="$(CFLAG)" TOPDIR=$(TOPDIR)

obj-y += main/XMLInstal.o
obj-y += main/loaderDL.o
obj-$(CONFIG_ALPHA_BLT) += platform/alpha_w55.o
obj-$(CONFIG_ALPHA_SOFT) += platform/alpha_soft.o
obj-y += main/thread_msg.o
obj-y += main/thread_timer.o
obj-y += main/thread_touch.o
obj-y += main/thread_keypad.o
obj-y += main/manager_timer.o
obj-y += main/manager_cs.o
obj-y += main/manager_touch.o
obj-y += main/schedule.o
obj-y += main/xmlproc.o
obj-y += main/layer.o
obj-y += main/screen_handler.o
obj-y += main/ParseXML.o
obj-y +=main/hulib.o
obj-y +=main/codec.o
obj-y += main/framebuffer.o
obj-y +=main/main.o  
obj-y += main/animation.o
obj-y +=platform/rotate_soft.o 
obj-y +=platform/zoom_soft.o 

obj-$(CONFIG_TOUCH_EKTF2K) += platform/touch_ektf2k.o
obj-$(CONFIG_TOUCH_TSLIB) += platform/touch_tslib.o
obj-$(CONFIG_TOUCH_NONE) += platform/touch_none.o
obj-$(CONFIG_KEYPAD_LITTLESWAN) += platform/keypad_littleSwan.o
obj-$(CONFIG_KEYPAD_NONE) += platform/keypad_none.o
ifeq ($(CONFIG_USING_FONT),y) 
	#LDFLAG+=-liconv 
	#obj-y += $(patsubst %.c,%.o,$(wildcard trueType/*.c))
	obj-$(CONFIG_USING_FONT) += main/ttf_font.o
	obj-$(CONFIG_USING_FONT) += main/transcoding.o
else
endif 


sinclude  $(MKAUTO)

obj-y += $(patsubst %.cpp,%.o,$(wildcard tinyxml/*.cpp))


OBJS := $(patsubst %.o,$(OBJS_DIR)%.o,$(obj-y))
DEPS := $(patsubst %.o,$(OBJS_DIR)%.d,$(obj-y))
dir-objs=$(patsubst %,%/built-in.o,$(dir-y))

OBJS_MK=$(CC) $(CFLAG) -c $< -o $@
%.o : %.c
	proc gcc
	@mkdir -p `dirname $@`
	$(CC) $(CFLAG) -c $< -o $@

%.o : %.cpp
	proc gcc
	@mkdir -p `dirname $@`
	$(CC) $(CFLAG) -c $< -o $@


all: .config lib/libz.a lib/libpng.a  lib/libfreetype.a dragon_auto $(DEPS) $(OBJS) dirobjs
	@echo built-in module: $(patsubst $(TOPDIR)%,%,$(dir-y))
	@$(CC)  $(OBJS) $(SRCS) lib/libpng.a lib/libz.a   lib/libfreetype.a $(dir-objs) -o $(TARGET) $(LDFLAG)
	@#rm $(dir-objs)
	@#$(STRIP) $(TARGET) 
	@echo build done 	

dragon_auto:
	@$(DRAGON_AUTO)



lib/libz.a:
	@mkdir -p `dirname $@`
	CROSS_COMPILE=$(CROSS_COMPILE) $(MKZLIB)
	rm $(TOPDIR)/bin -rf
	rm $(TOPDIR)/share -rf
lib/libpng.a:
	@mkdir -p `dirname $@`
	TOPDIR=$(TOPDIR) CC=$(CROSS_COMPILE)gcc $(MKPNGLIB)
	rm $(TOPDIR)/bin -rf
	rm $(TOPDIR)/share -rf

lib/libiconv.a:
	TOPDIR=$(TOPDIR) CC=$(CROSS_COMPILE)gcc $(MKICONVLIB)
	rm $(TOPDIR)/bin -rf
	rm $(TOPDIR)/share -rf

lib/libfreetype.a:
	TOPDIR=$(TOPDIR) CC=$(CROSS_COMPILE)gcc $(MKFREETYPELIB)
	rm $(TOPDIR)/bin -rf
	rm $(TOPDIR)/share -rf

.PHONY: tools
tools:
	$(MAKE) -C tools/

$(dir-objs):
	@mkdir -p `dirname $@`
	@$(MAKE) -C $(patsubst %built-in.o,%,$@) built-in.o

dirobjs:
	@for dir in $(dir-y);do echo build $$dir ;$(MAKE) -s -C $$dir built-in.o;done

$(MKAUTO):
	@$(DRAGON_AUTO)

$(MCONF) : 
	make OUTDIR=$(dir $@) -C script/kconfig

$(CONF) : 
	make OUTDIR=$(dir $@) -C script/kconfig

#.config: $(MCONF) $(CONF)
#	echo $(WIDGETDIR)
#	$(MCONF) Kconfig
#	$(CONF) -s Kconfig

.PHONY: menuconfig
menuconfig: $(MCONF) $(CONF) dragon_auto
	echo $(WIDGETDIR)
	$(MCONF) Kconfig
	$(CONF) -s Kconfig
clean:
	-rm -rf $(OBJS_DIR)
	-rm -f $(TARGET)	
	-@for dir in $(dir-y);do $(MAKE) -s -C $$dir clean;done

distclean: clean
	-rm $(MCONF)
	-rm $(CONF)
	-rm -rf lib

sinclude $(DEPS)

$(OBJS_DIR)%.d : %.cpp
	@mkdir -p `dirname $@`
	@set -e; rm -f $@
	@$(CC) $(CFLAG) -MM $(CPPFLAGS) $< |sed 's/\\//g' > $@
	@echo $(patsubst %.d,%.o,$@) : `cut -f2 -d':' <$@` > $@
	@printf "\t$(CC) $(CFLAG) -c $(patsubst %.d,%.cpp,$<) -o $(patsubst %.d,%.o,$@)\n" >>$@

$(OBJS_DIR)%.d : %.c
	@mkdir -p `dirname $@`
	@set -e; rm -f $@
	@$(CC) $(CFLAG) -MM $(CPPFLAGS) $< |sed 's/\\//g' > $@
	@echo $(patsubst %.d,%.o,$@) : `cut -f2 -d':' <$@` > $@
	@printf "\t$(CC) $(CFLAG) -c $(patsubst %.d,%.c,$<) -o $(patsubst %.d,%.o,$@)\n" >>$@
	
