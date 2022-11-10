TARGET = openpara

all:
	$(MAKE) -f Makefile.mkoverlay
	$(MAKE) -f Makefile.mkpsx TARGET=$(TARGET)
	$(MAKE) -f Makefile.mkiso TARGET=$(TARGET)

clean:
	$(MAKE) -f Makefile.mkoverlay clean
	$(MAKE) -f Makefile.mkpsx TARGET=$(TARGET) clean
	$(MAKE) -f Makefile.mkiso TARGET=$(TARGET) clean
