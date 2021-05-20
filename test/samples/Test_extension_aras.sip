2021-01-17 00:23:51,158634
NOTIFY sip:denver.peterson.lab@loopup.co;pool=il-ed-aras;box=il-ed-aras-05.ring2-corp.com SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__denver.peterson.lab@loopup.co;received=127.0.0.1:48944:1152121280
Via: SIP/2.0/TCP 127.0.0.1:49270;received=127.0.0.1:49270:12683536
Call-ID: 971092900532354310.il-ed-aras-01.ring2-corp.com
CSeq: 1 NOTIFY
From: sip:conference.wizard@local.host;pool=il-ed-aras;box=il-ed-aras-05.ring2-corp.com
To: sip:denver.peterson.lab@loopup.co;pool=il-ed-aras;box=il-ed-aras-05.ring2-corp.com
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: CallStart
X-slave-site: localhost
X-last-change: 1610843031
X-Conf_no: 9038
X-rollcall: disabled
X-Call-URL: true
X-control-master: denver.peterson.lab@loopup.co
X-domain: DEFAULT
X-client-address: 10.1.224.108
X-From: denver.peterson.lab@loopup.co
X-legs-on-server: 1
X-start-muted: false
X-Video-SingleView: 0
X-suppress-system-im: false
X-restrict-notify: false
X-notify-im: false
X-no-audio: false
X-no-video: false
X-dialout: allowed
X-Billing-code-required: false
X-recording-enabled: true
X-Video-UsingMCU: false
X-subject: Denver's Lab Meeting Room
X-Call-Instance-ID: OTcxMDkyOTAwNTMyMzU0MzEwLmlsLWVkLWFyYXMtMDEucmluZzItY29ycC5jb206MTYxMDg0MzAzMDo5MDgwODY=
X-Route-ID: 2
Content-Length: 1153
Content-type: application/sdp

v=0
o=denver.peterson.lab@loopup.co 245501968 104 IN IP4 il-ed-aras-01.ring2-corp.com
s=Denver's Lab Meeting Room
i="Denver%20Peterson" (+14152264822x,0830423985) CallByPhone-URL
c=TN RFC2543 +14152264822x,0830423985
t=3819831831 0
m=audio 1 voice -
a=status:(2) starting
a=mediastatus:sendrecv
a=leg_no:1
a=flags:1054240
a=new_change
a=server:il-ed-aras-05.ring2-corp.com
a=clir:false:14154498508
a=acs_guid:000958503951
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:denver.peterson.lab@loopup.co
a=legCallid:1222743542@67.131.238.73
a=cdr_start_time:1610843031.087070
a=preferred_identity:14154498508
a=x-ring2-callowner-login_alias:denver.peterson.lab@loopup.co
a=x-ring2-legowner-login_alias:denver.peterson.lab@loopup.co
a=x-ring2-alt-proj: 
a=x-ring2-call_source_type:0
a=x-ring2-displayname:Denver%20Peterson
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:135.180.107.115:24051
a=x-ring2-unique_device_id:4c52138a-e4ac-42d3-986d-2df18cc15b69
a=x-ring2-UF1: 
a=x-ring2-coords:1.11
a=ivr:dialout
a=dialout:click_in
a=access_code:2451228
a=privs:leader
a=x-ring2-smartproxy:USAZW2-LALTAIR1.ring2-corp.com


2021-01-17 00:24:08,032310
NOTIFY sip:denver.peterson.lab@loopup.co;pool=il-ed-aras;box=il-ed-aras-05.ring2-corp.com SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__denver.peterson.lab@loopup.co;received=127.0.0.1:48944:1152121280
Via: SIP/2.0/TCP 127.0.0.1:49270;received=127.0.0.1:49270:12683536
Call-ID: 971092900532354310.il-ed-aras-01.ring2-corp.com
CSeq: 3 NOTIFY
From: sip:conference.wizard@local.host;pool=il-ed-aras;box=il-ed-aras-05.ring2-corp.com
To: sip:denver.peterson.lab@loopup.co;pool=il-ed-aras;box=il-ed-aras-05.ring2-corp.com
Contact: <sip:localhost:8443;transport=ssl>
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1610843048
X-Conf_no: 9038
X-rollcall: disabled
X-Call-URL: true
X-control-master: denver.peterson.lab@loopup.co
X-domain: DEFAULT
X-client-address: 10.1.224.108
X-From: denver.peterson.lab@loopup.co
X-legs-on-server: 1
X-start-muted: false
X-Video-SingleView: 0
X-suppress-system-im: false
X-restrict-notify: false
X-notify-im: false
X-no-audio: false
X-no-video: false
X-dialout: allowed
X-Billing-code-required: false
X-recording-enabled: true
X-Video-UsingMCU: false
X-subject: Denver's Lab Meeting Room
X-Call-Instance-ID: OTcxMDkyOTAwNTMyMzU0MzEwLmlsLWVkLWFyYXMtMDEucmluZzItY29ycC5jb206MTYxMDg0MzAzMDo5MDgwODY=
X-Route-ID: 2
Content-Length: 1302
Content-type: application/sdp

v=0
o=denver.peterson.lab@loopup.co 245501968 115 IN IP4 il-ed-aras-01.ring2-corp.com
s=Denver's Lab Meeting Room
i="Denver%20Peterson" (+14152264822x,0830423985) CallByPhone-URL
c=TN RFC2543 +14152264822x,0830423985
t=3819831847 0
m=audio 1 voice -
a=status:(203) answered
a=mediastatus:sendrecv
a=leg_no:1
a=remote:67.131.238.73:12000
a=flags:1054240
a=new_change
a=server:il-ed-aras-05.ring2-corp.com
a=clir:false:14154498508
a=far_end:216.111.92.72:18350
a=acs_guid:000958503951
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:denver.peterson.lab@loopup.co
a=legCallid:1222743542@67.131.238.73
a=audio_payload:PCMU
a=cdr_start_time:1610843031.087070
a=preferred_identity:14154498508
a=x-ring2-callowner-login_alias:denver.peterson.lab@loopup.co
a=x-ring2-legowner-login_alias:denver.peterson.lab@loopup.co
a=x-ring2-alt-proj: 
a=x-ring2-call_source_type:0
a=x-ring2-displayname:Denver%20Peterson
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:135.180.107.115:24051
a=x-ring2-unique_device_id:4c52138a-e4ac-42d3-986d-2df18cc15b69
a=x-ring2-UF1: 
a=x-ring2-coords:2.11
a=x-ring2-coords:2.22
a=ivr:dialout
a=dialout:click_in
a=access_code:2451228
a=privs:leader
a=x-ring2-smartproxy:USAZW2-LALTAIR1.ring2-corp.com
a=user-agent:Cisco-SIPGateway/IOS-12.x


2021-01-17 00:26:31,457767
NOTIFY sip:denver.peterson.lab@loopup.co;pool=il-ed-aras;box=il-ed-aras-05.ring2-corp.com SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__denver.peterson.lab@loopup.co;received=127.0.0.1:48944:1152121280
Via: SIP/2.0/TCP 127.0.0.1:49270;received=127.0.0.1:49270:12683536
Call-ID: 971092900532354310.il-ed-aras-01.ring2-corp.com
CSeq: 31 NOTIFY
From: sip:conference.wizard@local.host;pool=il-ed-aras;box=il-ed-aras-05.ring2-corp.com
To: sip:denver.peterson.lab@loopup.co;pool=il-ed-aras;box=il-ed-aras-05.ring2-corp.com
Contact: <sip:localhost:8443;transport=ssl>
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1610843191
X-Conf_no: 9038
X-rollcall: disabled
X-Call-URL: true
X-control-master: denver.peterson.lab@loopup.co
X-domain: DEFAULT
X-client-address: 10.1.224.108
X-From: denver.peterson.lab@loopup.co
X-legs-on-server: 1
X-start-muted: false
X-Video-SingleView: 0
X-suppress-system-im: false
X-restrict-notify: false
X-notify-im: false
X-no-audio: false
X-no-video: false
X-dialout: allowed
X-Billing-code-required: false
X-recording-enabled: true
X-Video-UsingMCU: false
X-subject: Denver's Lab Meeting Room
X-Call-Instance-ID: OTcxMDkyOTAwNTMyMzU0MzEwLmlsLWVkLWFyYXMtMDEucmluZzItY29ycC5jb206MTYxMDg0MzAzMDo5MDgwODY=
X-Route-ID: 2
Content-Length: 1355
Content-type: application/sdp

v=0
o=denver.peterson.lab@loopup.co 245501968 153 IN IP4 il-ed-aras-01.ring2-corp.com
s=Denver's Lab Meeting Room
i="Denver%20Peterson" (+14152264822x,0830423985) CallByPhone-URL
c=TN RFC2543 +14152264822x,0830423985
t=3819831847 0
m=audio 1 voice -
a=status:(12) ending
a=mediastatus:sendrecv
a=cause: (30009) dropped by TNS
a=leg_no:1
a=remote:67.131.238.73:12000
a=flags:1054242
a=new_change
a=server:il-ed-aras-05.ring2-corp.com
a=clir:false:14154498508
a=far_end:216.111.92.72:18350
a=acs_guid:000958503951
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:denver.peterson.lab@loopup.co
a=legCallid:1222743542@67.131.238.73
a=audio_payload:PCMU
a=cdr_start_time:1610843031.087070
a=preferred_identity:14154498508
a=x-ring2-callowner-login_alias:denver.peterson.lab@loopup.co
a=x-ring2-legowner-login_alias:denver.peterson.lab@loopup.co
a=x-ring2-alt-proj: 
a=x-ring2-call_source_type:0
a=x-ring2-displayname:Denver%20Peterson
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:135.180.107.115:24051
a=x-ring2-unique_device_id:4c52138a-e4ac-42d3-986d-2df18cc15b69
a=x-ring2-UF1: 
a=x-ring2-coords:3.11
a=x-ring2-coords:3.22
a=x-ring2-coords:3.33
a=ivr:dialout
a=dialout:click_in
a=access_code:2451228
a=privs:leader
a=x-ring2-smartproxy:USAZW2-LALTAIR1.ring2-corp.com
a=user-agent:Cisco-SIPGateway/IOS-12.x
