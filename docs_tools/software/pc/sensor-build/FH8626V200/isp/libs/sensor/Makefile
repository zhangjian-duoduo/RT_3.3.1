-include ../personal.make

all:
	@if [ -f 'genversion.sh' ];then bash genversion.sh; fi
	@if [ -f 'gen_support_fmt.py' ];then python3 gen_support_fmt.py NULL; fi
	@for x in `find ./ -maxdepth 2 -mindepth 2 -name "Makefile" | sort`; \
	do cd `dirname $$x`; if [ $$? ]; then make; cd ../; fi; done
	@if [ -f 'gen_support_fmt.py' ];then python3 gen_support_fmt.py copy; fi

lib:
	@if [ -f 'genversion.sh' ];then bash genversion.sh; fi
	@for x in `find ./ -maxdepth 2 -mindepth 2 -name "Makefile" | sort`; \
	do cd `dirname $$x`; if [ $$? ]; then make lib; cd ../; fi; done

clean:
	@for x in `find ./ -maxdepth 2 -mindepth 2 -name "Makefile" | sort`; \
	do cd `dirname $$x`; if [ $$? ]; then make clean; cd ../; fi; done

install:
	@for x in `find ./ -maxdepth 2 -mindepth 2 -name "Makefile" | sort`; \
	do cd `dirname $$x`; if [ $$? ]; then make install; cd ../; fi; done

prebuild-paraFile:
	@for x in `find ./ -maxdepth 2 -mindepth 2 -name "Makefile" | sort`; \
	do cd `dirname $$x`; if [ $$? ]; then make prebuild-paraFile; cd ../; fi; done

build-paraFile:
	@for x in `find ./ -maxdepth 2 -mindepth 2 -name "Makefile" | sort`; \
	do cd `dirname $$x`; if [ $$? ]; then make build-paraFile; cd ../; fi; done

delete-paraFile:
	@for x in `find ./ -maxdepth 2 -mindepth 2 -name "Makefile" | sort`; \
	do cd `dirname $$x`; if [ $$? ]; then make delete-paraFile; cd ../; fi; done