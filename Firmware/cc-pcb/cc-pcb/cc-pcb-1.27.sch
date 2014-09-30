EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:special
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:74xgxx
LIBS:ac-dc
LIBS:brooktre
LIBS:cmos_ieee
LIBS:dc-dc
LIBS:elec-unifil
LIBS:ftdi
LIBS:gennum
LIBS:graphic
LIBS:hc11
LIBS:logo
LIBS:microchip_pic10mcu
LIBS:microchip_pic12mcu
LIBS:microchip_pic16mcu
LIBS:microchip1
LIBS:msp430
LIBS:nxp_armmcu
LIBS:powerint
LIBS:pspice
LIBS:references
LIBS:relays
LIBS:rfcom
LIBS:sensors
LIBS:stm8
LIBS:stm32
LIBS:supertex
LIBS:transf
LIBS:ttl_ieee
LIBS:video
LIBS:w_analog
LIBS:w_connectors
LIBS:w_device
LIBS:w_logic
LIBS:w_memory
LIBS:w_microcontrollers
LIBS:w_opto
LIBS:w_power
LIBS:w_relay
LIBS:w_rtx
LIBS:w_transistor
LIBS:w_vacuum
LIBS:cc-pcb-1.27-cache
EELAYER 27 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "cc-schema.sch"
Date "30 sep 2014"
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L ATMEGA328P-A IC1
U 1 1 53F3A6E6
P 3200 3100
F 0 "IC1" H 2450 4350 40  0000 L BNN
F 1 "ATMEGA328P-A" H 3600 1700 40  0000 L BNN
F 2 "TQFP32" H 3200 3100 30  0000 C CIN
F 3 "" H 3200 3100 60  0000 C CNN
	1    3200 3100
	1    0    0    -1  
$EndComp
$Comp
L DIODE D1
U 1 1 53F3AA52
P 8100 2600
F 0 "D1" H 8100 2700 40  0000 C CNN
F 1 "DIODE" H 8100 2500 40  0000 C CNN
F 2 "~" H 8100 2600 60  0000 C CNN
F 3 "~" H 8100 2600 60  0000 C CNN
	1    8100 2600
	0    -1   -1   0   
$EndComp
$Comp
L DIODE D2
U 1 1 53F3AAD7
P 7800 2700
F 0 "D2" H 7800 2800 40  0000 C CNN
F 1 "DIODE" H 7800 2600 40  0000 C CNN
F 2 "~" H 7800 2700 60  0000 C CNN
F 3 "~" H 7800 2700 60  0000 C CNN
	1    7800 2700
	0    -1   -1   0   
$EndComp
$Comp
L DIODE D3
U 1 1 53F3ACD2
P 7500 2800
F 0 "D3" H 7500 2900 40  0000 C CNN
F 1 "DIODE" H 7500 2700 40  0000 C CNN
F 2 "~" H 7500 2800 60  0000 C CNN
F 3 "~" H 7500 2800 60  0000 C CNN
	1    7500 2800
	0    -1   -1   0   
$EndComp
$Comp
L SW_PUSH SW1
U 1 1 53FA22FD
P 4650 1000
F 0 "SW1" H 4800 1110 50  0000 C CNN
F 1 "SRS" H 4650 920 50  0000 C CNN
F 2 "~" H 4650 1000 60  0000 C CNN
F 3 "~" H 4650 1000 60  0000 C CNN
	1    4650 1000
	1    0    0    -1  
$EndComp
$Comp
L CONN_6 P5
U 1 1 53FA2625
P 2450 5550
F 0 "P5" V 2400 5550 60  0000 C CNN
F 1 "CONN_6" V 2500 5550 60  0000 C CNN
F 2 "" H 2450 5550 60  0000 C CNN
F 3 "" H 2450 5550 60  0000 C CNN
	1    2450 5550
	-1   0    0    -1  
$EndComp
$Comp
L BARREL_JACK CON1
U 1 1 53FA2BF3
P 2250 950
F 0 "CON1" H 2250 1200 60  0000 C CNN
F 1 "BARREL_JACK" H 2250 750 60  0000 C CNN
F 2 "" H 2250 950 60  0000 C CNN
F 3 "" H 2250 950 60  0000 C CNN
	1    2250 950 
	1    0    0    -1  
$EndComp
$Comp
L C C2
U 1 1 53FCE3B4
P 3950 5100
F 0 "C2" H 3950 5200 40  0000 L CNN
F 1 "0.1uF" H 3956 5015 40  0000 L CNN
F 2 "~" H 3988 4950 30  0000 C CNN
F 3 "~" H 3950 5100 60  0000 C CNN
	1    3950 5100
	1    0    0    -1  
$EndComp
$Comp
L R R1
U 1 1 53FCE49F
P 3550 4750
F 0 "R1" V 3630 4750 40  0000 C CNN
F 1 "10k" V 3557 4751 40  0000 C CNN
F 2 "~" V 3480 4750 30  0000 C CNN
F 3 "~" H 3550 4750 30  0000 C CNN
	1    3550 4750
	0    -1   -1   0   
$EndComp
$Comp
L CONN_6 P6
U 1 1 53FCECA3
P 2450 6650
F 0 "P6" V 2400 6650 60  0000 C CNN
F 1 "CONN_6" V 2500 6650 60  0000 C CNN
F 2 "" H 2450 6650 60  0000 C CNN
F 3 "" H 2450 6650 60  0000 C CNN
	1    2450 6650
	-1   0    0    -1  
$EndComp
$Comp
L CP1 C1
U 1 1 53FCF596
P 3350 1000
F 0 "C1" H 3400 1100 50  0000 L CNN
F 1 "10uF" H 3400 900 50  0000 L CNN
F 2 "~" H 3350 1000 60  0000 C CNN
F 3 "~" H 3350 1000 60  0000 C CNN
	1    3350 1000
	1    0    0    -1  
$EndComp
$Comp
L DIODE D6
U 1 1 53F3ACF3
P 6600 3100
F 0 "D6" H 6600 3200 40  0000 C CNN
F 1 "DIODE" H 6600 3000 40  0000 C CNN
F 2 "~" H 6600 3100 60  0000 C CNN
F 3 "~" H 6600 3100 60  0000 C CNN
	1    6600 3100
	0    -1   -1   0   
$EndComp
$Comp
L DIODE D5
U 1 1 53F3ACEC
P 6900 3000
F 0 "D5" H 6900 3100 40  0000 C CNN
F 1 "DIODE" H 6900 2900 40  0000 C CNN
F 2 "~" H 6900 3000 60  0000 C CNN
F 3 "~" H 6900 3000 60  0000 C CNN
	1    6900 3000
	0    -1   -1   0   
$EndComp
$Comp
L DIODE D4
U 1 1 53F3ACD8
P 7200 2900
F 0 "D4" H 7200 3000 40  0000 C CNN
F 1 "DIODE" H 7200 2800 40  0000 C CNN
F 2 "~" H 7200 2900 60  0000 C CNN
F 3 "~" H 7200 2900 60  0000 C CNN
	1    7200 2900
	0    -1   -1   0   
$EndComp
NoConn ~ 4200 2600
NoConn ~ 2300 3350
NoConn ~ 2300 3450
$Comp
L VCC #PWR01
U 1 1 54048C90
P 2700 700
F 0 "#PWR01" H 2700 800 30  0001 C CNN
F 1 "VCC" H 2700 800 30  0000 C CNN
F 2 "" H 2700 700 60  0000 C CNN
F 3 "" H 2700 700 60  0000 C CNN
	1    2700 700 
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 54048CAD
P 2700 1300
F 0 "#PWR02" H 2700 1300 30  0001 C CNN
F 1 "GND" H 2700 1230 30  0000 C CNN
F 2 "" H 2700 1300 60  0000 C CNN
F 3 "" H 2700 1300 60  0000 C CNN
	1    2700 1300
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR03
U 1 1 54048F4B
P 2200 1800
F 0 "#PWR03" H 2200 1900 30  0001 C CNN
F 1 "VCC" H 2200 1900 30  0000 C CNN
F 2 "" H 2200 1800 60  0000 C CNN
F 3 "" H 2200 1800 60  0000 C CNN
	1    2200 1800
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR04
U 1 1 54048F81
P 3100 4750
F 0 "#PWR04" H 3100 4850 30  0001 C CNN
F 1 "VCC" H 3100 4850 30  0000 C CNN
F 2 "" H 3100 4750 60  0000 C CNN
F 3 "" H 3100 4750 60  0000 C CNN
	1    3100 4750
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR05
U 1 1 54048FCE
P 3300 7100
F 0 "#PWR05" H 3300 7100 30  0001 C CNN
F 1 "GND" H 3300 7030 30  0000 C CNN
F 2 "" H 3300 7100 60  0000 C CNN
F 3 "" H 3300 7100 60  0000 C CNN
	1    3300 7100
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR06
U 1 1 54048FE9
P 5500 6000
F 0 "#PWR06" H 5500 6100 30  0001 C CNN
F 1 "VCC" H 5500 6100 30  0000 C CNN
F 2 "" H 5500 6000 60  0000 C CNN
F 3 "" H 5500 6000 60  0000 C CNN
	1    5500 6000
	-1   0    0    1   
$EndComp
$Comp
L GND #PWR07
U 1 1 54048FF8
P 5800 6050
F 0 "#PWR07" H 5800 6050 30  0001 C CNN
F 1 "GND" H 5800 5980 30  0000 C CNN
F 2 "" H 5800 6050 60  0000 C CNN
F 3 "" H 5800 6050 60  0000 C CNN
	1    5800 6050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR08
U 1 1 5404988F
P 2200 4450
F 0 "#PWR08" H 2200 4450 30  0001 C CNN
F 1 "GND" H 2200 4380 30  0000 C CNN
F 2 "" H 2200 4450 60  0000 C CNN
F 3 "" H 2200 4450 60  0000 C CNN
	1    2200 4450
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR09
U 1 1 54049D68
P 2900 5900
F 0 "#PWR09" H 2900 5900 30  0001 C CNN
F 1 "GND" H 2900 5830 30  0000 C CNN
F 2 "" H 2900 5900 60  0000 C CNN
F 3 "" H 2900 5900 60  0000 C CNN
	1    2900 5900
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR010
U 1 1 54049E65
P 2900 5150
F 0 "#PWR010" H 2900 5250 30  0001 C CNN
F 1 "VCC" H 2900 5250 30  0000 C CNN
F 2 "" H 2900 5150 60  0000 C CNN
F 3 "" H 2900 5150 60  0000 C CNN
	1    2900 5150
	1    0    0    -1  
$EndComp
$Comp
L VCC #PWR011
U 1 1 54049FAB
P 3300 6200
F 0 "#PWR011" H 3300 6300 30  0001 C CNN
F 1 "VCC" H 3300 6300 30  0000 C CNN
F 2 "" H 3300 6200 60  0000 C CNN
F 3 "" H 3300 6200 60  0000 C CNN
	1    3300 6200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR012
U 1 1 5404A234
P 4950 1200
F 0 "#PWR012" H 4950 1200 30  0001 C CNN
F 1 "GND" H 4950 1130 30  0000 C CNN
F 2 "" H 4950 1200 60  0000 C CNN
F 3 "" H 4950 1200 60  0000 C CNN
	1    4950 1200
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG013
U 1 1 5405B6E7
P 2700 700
F 0 "#FLG013" H 2700 795 30  0001 C CNN
F 1 "PWR_FLAG" H 2700 880 30  0000 C CNN
F 2 "" H 2700 700 60  0000 C CNN
F 3 "" H 2700 700 60  0000 C CNN
	1    2700 700 
	0    -1   -1   0   
$EndComp
$Comp
L PWR_FLAG #FLG014
U 1 1 5405B6F6
P 2700 1300
F 0 "#FLG014" H 2700 1395 30  0001 C CNN
F 1 "PWR_FLAG" H 2700 1480 30  0000 C CNN
F 2 "" H 2700 1300 60  0000 C CNN
F 3 "" H 2700 1300 60  0000 C CNN
	1    2700 1300
	0    -1   -1   0   
$EndComp
NoConn ~ 2300 2600
$Comp
L CONN_2 P4
U 1 1 5411AEED
P 6450 5650
F 0 "P4" V 6400 5650 40  0000 C CNN
F 1 "CONN_2" V 6500 5650 40  0000 C CNN
F 2 "" H 6450 5650 60  0000 C CNN
F 3 "" H 6450 5650 60  0000 C CNN
	1    6450 5650
	1    0    0    -1  
$EndComp
$Comp
L CONN_20 P1
U 1 1 5424217B
P 8950 4150
F 0 "P1" V 8900 4150 60  0000 C CNN
F 1 "CONN_20" V 9000 4150 60  0000 C CNN
F 2 "" H 8950 4150 60  0000 C CNN
F 3 "" H 8950 4150 60  0000 C CNN
	1    8950 4150
	1    0    0    -1  
$EndComp
Connection ~ 2200 4300
Wire Wire Line
	2200 4300 2300 4300
Connection ~ 2200 4200
Wire Wire Line
	2300 4200 2200 4200
Wire Wire Line
	2200 4100 2200 4450
Wire Wire Line
	2300 4100 2200 4100
Wire Wire Line
	5800 5750 6100 5750
Wire Wire Line
	5800 6050 5800 5750
Wire Wire Line
	5500 5550 6100 5550
Wire Wire Line
	5500 6000 5500 5550
Wire Wire Line
	4200 4300 5100 4300
Wire Wire Line
	4200 4200 5200 4200
Wire Wire Line
	4200 4100 5300 4100
Wire Wire Line
	5400 4000 5400 5000
Wire Wire Line
	4200 4000 5400 4000
Wire Wire Line
	5500 3900 5500 5100
Wire Wire Line
	4200 3900 5500 3900
Wire Wire Line
	5600 3800 5600 4900
Connection ~ 4300 3700
Wire Wire Line
	5700 3700 5700 4700
Wire Wire Line
	5800 3600 5800 4500
Wire Wire Line
	4200 3600 5800 3600
Connection ~ 4600 2300
Connection ~ 4700 2500
Connection ~ 4800 2400
Wire Wire Line
	3300 6900 3300 7100
Wire Wire Line
	2800 6900 3300 6900
Connection ~ 4500 4750
Wire Wire Line
	4500 6800 2800 6800
Wire Wire Line
	4600 2300 4600 6700
Wire Wire Line
	4600 6700 2800 6700
Wire Wire Line
	4700 2500 4700 6600
Wire Wire Line
	4700 6600 2800 6600
Wire Wire Line
	4800 2400 4800 6400
Wire Wire Line
	4800 6400 2800 6400
Connection ~ 3950 4750
Wire Wire Line
	3950 4750 3950 4900
Wire Wire Line
	4500 3450 4200 3450
Wire Wire Line
	4500 3450 4500 6800
Wire Wire Line
	3800 4750 4500 4750
Wire Wire Line
	3100 4750 3300 4750
Wire Wire Line
	2800 5300 3950 5300
Wire Wire Line
	2200 2300 2300 2300
Wire Wire Line
	2700 700  2700 850 
Wire Wire Line
	2700 850  2550 850 
Connection ~ 2700 1050
Wire Wire Line
	2550 1050 2700 1050
Wire Wire Line
	2700 950  2700 1300
Wire Wire Line
	2550 950  2700 950 
Wire Wire Line
	4200 3800 5600 3800
Wire Wire Line
	4400 3600 4400 5500
Wire Wire Line
	4400 5500 2800 5500
Wire Wire Line
	4200 3700 5700 3700
Wire Wire Line
	4300 3700 4300 5400
Wire Wire Line
	4300 5400 2800 5400
Wire Wire Line
	4200 2700 4350 2700
Wire Wire Line
	4350 2700 4350 1000
Wire Wire Line
	4200 2300 7200 2300
Wire Wire Line
	2800 5700 2900 5700
Wire Wire Line
	2900 5700 2900 5900
Wire Wire Line
	2800 5800 2900 5800
Connection ~ 2900 5800
Wire Wire Line
	2800 5600 2900 5600
Wire Wire Line
	2900 5600 2900 5150
Wire Wire Line
	2800 6500 3300 6500
Wire Wire Line
	3300 6500 3300 6200
Wire Wire Line
	4950 1000 4950 1200
Wire Wire Line
	3350 800  3350 750 
Wire Wire Line
	3350 750  2700 750 
Connection ~ 2700 750 
Wire Wire Line
	3350 1200 3350 1250
Wire Wire Line
	3350 1250 2700 1250
Connection ~ 2700 1250
Wire Wire Line
	2200 1800 2200 2300
Wire Wire Line
	2300 2100 2200 2100
Connection ~ 2200 2100
Wire Wire Line
	2300 2000 2200 2000
Connection ~ 2200 2000
Connection ~ 4400 3600
Wire Wire Line
	4200 3350 5900 3350
Wire Wire Line
	5900 3350 5900 4300
Wire Wire Line
	5900 4300 8600 4300
Wire Wire Line
	4200 3250 6000 3250
Wire Wire Line
	6000 3250 6000 4200
Wire Wire Line
	6000 4200 8600 4200
Wire Wire Line
	4200 3150 6100 3150
Wire Wire Line
	6100 3150 6100 4100
Wire Wire Line
	6100 4100 8600 4100
Wire Wire Line
	4200 3050 6200 3050
Wire Wire Line
	6200 3050 6200 4000
Wire Wire Line
	6200 4000 8600 4000
Wire Wire Line
	4200 2950 6300 2950
Wire Wire Line
	6300 2950 6300 3900
Wire Wire Line
	6300 3900 8600 3900
Wire Wire Line
	4200 2850 6400 2850
Wire Wire Line
	6400 2850 6400 3800
Wire Wire Line
	6400 3800 8600 3800
Wire Wire Line
	4200 2500 6600 2500
Wire Wire Line
	6600 2500 6600 2900
Wire Wire Line
	6600 3300 6600 3700
Wire Wire Line
	6600 3700 8600 3700
Wire Wire Line
	4200 2400 6900 2400
Wire Wire Line
	6900 2400 6900 2800
Wire Wire Line
	7200 2300 7200 2700
Wire Wire Line
	4200 2200 7500 2200
Wire Wire Line
	7500 2200 7500 2600
Wire Wire Line
	4200 2100 7800 2100
Wire Wire Line
	7800 2100 7800 2500
Wire Wire Line
	4200 2000 8100 2000
Wire Wire Line
	8100 2000 8100 2400
Wire Wire Line
	7200 3100 7200 3500
Wire Wire Line
	7200 3500 8600 3500
Wire Wire Line
	7800 2900 7800 3400
Wire Wire Line
	8100 2800 8100 3600
Wire Wire Line
	5800 4500 8600 4500
Wire Wire Line
	5700 4700 8600 4700
Wire Wire Line
	5600 4900 8600 4900
Wire Wire Line
	5500 5100 8600 5100
Wire Wire Line
	5400 5000 8600 5000
Wire Wire Line
	5300 4100 5300 4800
Wire Wire Line
	5300 4800 8600 4800
Wire Wire Line
	5200 4200 5200 4600
Wire Wire Line
	5200 4600 8600 4600
Wire Wire Line
	5100 4300 5100 4400
Wire Wire Line
	5100 4400 8600 4400
Wire Wire Line
	7500 3000 7500 3200
Wire Wire Line
	7500 3200 8600 3200
Wire Wire Line
	7800 3400 8600 3400
Wire Wire Line
	8100 3600 8600 3600
Wire Wire Line
	6900 3200 6900 3300
Wire Wire Line
	6900 3300 8600 3300
$EndSCHEMATC
