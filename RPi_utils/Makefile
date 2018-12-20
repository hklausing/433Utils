TARGETS		= send codesend RFSniffer
RCSWITCH	= ../rc-switch/RCSwitch.o
HELPER		= helper.o
INSTPATH	= /usr/local/bin

# Defines the RPI variable which is needed by rc-switch/RCSwitch.h
CXXFLAGS=-DRPI

#all: send codesend RFSniffer
all: $(TARGETS)

send: $(RCSWITCH) $(HELPER) send.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $+ -o $@ -lwiringPi -lwiringPiDev -lcrypt

codesend: $(RCSWITCH) $(HELPER) codesend.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $+ -o $@ -lwiringPi -lwiringPiDev -lcrypt

RFSniffer: $(RCSWITCH) $(HELPER) RFSniffer.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $+ -o $@ -lwiringPi -lwiringPiDev -lcrypt


install:
	cp $(TARGETS) $(INSTPATH)/


clean:
	$(RM) $(addprefix $(INSTPATH)/,$(TARGETS)) \
		$(addprefix ./,$(TARGETS)) \
		$(addsuffix .o,$(TARGETS)) \
		$(RCSWITCH) $(HELPER)


#../rc-switch/*.o *.o send codesend servo RFSniffer
