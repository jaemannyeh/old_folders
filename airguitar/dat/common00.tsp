
chipx##/cx##	With chipx1 or cx1 meaning chip rate (1.2288 MHz), chipx8 (cx8) means 8 times chip rate or 19.6608 MHz, chipx16 (cx16) means 16 times chip rate or 39.3216 MHz, etc.	s
CSM	Cell Site Modem	s
FL	Forward link	s
MSM	Mobile Station Modem	s
PCG	Power control group	s
PN	Pseudonoise	s
PS	Processing slot	s
RC	Radio configuration	s
RL	Reverse link	s
RS	Rate set. Applies to IS-95-A/B modes and is replaced in IS-2000 with RC	s
SADV	Slot advance (prefix used on various signals)	s
SR1	Spreading rate 1	s
SR3	Spreading rate 3	s
WLL	Wireless local loop	s
WS	Walsh symbol	s

F-PICH	Forward pilot channel	s
F-TDPICH	Forward transmit-diversity channel	s
F-APICH	Forward auxiliary-pilot channel	s
F-ATDPICH	Forward auxiliary transmit-diversity channel	s
F-SYNCH	Forward sync channel	s
F-PCH	Forward paging channel	s
F-BCH	Forward broadcast channel	s
F-QPCH	Forward quick-paging channel	s
F-CPCCH	Forward common-power control channel	s
F-CACH	Forward common-assignment channel	s
F-CCCH	Forward common-control channel	s
F-DCCH	Forward dedicated-control channel	s
F-FCH	Forward fundamental channel	s
F-SCH	Forward supplemental channel	s
F-SCCH	Forward supplemental-code channel	s
R-ACH	Reverse-access channel	s
R-EACH	Reverse enhanced-access channel	s
R-CCCH	Reverse common-control channel	s
R-DCCH	Reverse dedicated-control channel	s
R-FCH	Reverse fundamental channel	s
R-SCH	Reverse supplemental channel	s
R-SCCH	Reverse supplemental-code channel	s
R-SPICH	Reverse secondary-pilot channel	s

PCG	1.25 ms 6 Walsh symbols (RC1/2 RL) 1536 PN chips	s
WalshSymbol	208.3 us 256 PN chips (RC1/2 RL) 62.5 km in free space	s
ProcessingCycle	208.3 us 256 PN chips (768 processing slots when clk_sys is at chipx96)	s
WalshChip	3.255 us 4 PN chips (RC1/2 RL)	s
chipx1	813.8 ns 2 chipx2s (PN chip) 244 m in free space	s
chipx2	406.9 ns 4 chipx8s	s
ProcessingSlot	232.5 ns 32 clk_sys	s
chipx8	101.725 ns 2 chipx16s	s
chipx16	50.863 ns 2 chipx32s	s
chipx32	25.431 ns -	s
clk_sys	8.477 ns 118 MHz at chipx96	s

local_time
local_time_zone
daylight_saving_begin
daylight_saving_end

cpu_ctime
csm_ctime
gps_ctime

build_version	0.0.0
board_version	0.0.0
fpga_version	0.0.0
bootrom_version	0.0.0

gps_temperature	43.4224	f
gps_latitude	0.5755	f
gps_longitude	-1.6880	f
gps_elevation	160.5748	f
gps_utc_offset	0	i
gps_time_of_week	0	i

ntp_ip_addr	0.0.0.0
bsm_ip_addr	0.0.0.0
fsm_ip_addr	0.0.0.0

eth0_mtu	1500	i
eth0_speed	100	i
eth0_duplex	full
eth0_mac_addr	00:00:00:00:00:00
eth0_ip_addr	0.0.0.0
eth0_ip_gw_addr	0.0.0.0
eth0_ipsec_gw_addr	0.0.0.0

user0_name
user0_password
ftp_user0_name
ftp_user0_password
ssh_user0_name
ssh_user0_password
telnet_user0_name
telnet_user0_password

bs_id
bs_model
bs_vendor
bs_carrier
bts_id
bsc_id

ftp_tcp_port	21	i
ssh_tcp_port	22	i
telnet_tcp_port	23	i
bootpc_udp_port	68	i
udp_port_3200	3200	i
udp_port_3201	3201	i
udp_port_4500	4500	i
sip_udp_port	5060	i
udp_port_34000	34000	i
udp_port_34001	34001	i
udp_port_34003	34003	i
udp_port_34004	34004	i
udp_port_34005	34005	i

####################### reverse    forward   ba  se
bc0	cellular   800  815..849   860..894  34  45
bc1	pcs       1900 1850..1910 1930..1990 60  80
bc4	korea pcs 1800 1750..1780 1840..1870 30  90
bc6	imt       2100 1920..1980 2110..2170 60 190
bc14	us pcs    1900 1850..1915 1930..1995 65  80

band_class	bc0

onex_band_class
onex_rssi
onex_tssi
onex_rx_attenuation
onex_rx_channel
onex_tx_attenuation
onex_tx_channel

evdo_band_class
evdo_rssi
evdo_tssi
evdo_rx_attenuation
evdo_rx_channel
evdo_tx_attenuation
evdo_tx_channel

onex_pdsn_number
onex_pdsn_0_index
onex_pdsn_0_ios_version
onex_pdsn_0_protocol
onex_pdsn_0_ip_addr
onex_pdsn_0_udp_port	699	i

evdo_pdsn_number
evdo_pdsn_0_index
evdo_pdsn_0_ios_version
evdo_pdsn_0_protocol
evdo_pdsn_0_ip_addr
evdo_pdsn_0_udp_port	699	i
