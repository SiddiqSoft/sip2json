NOTIFY sip:user.dp.lab@example.org;pool=il-ed-aras;box=il-ed-aras-05.example-corp.com SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__user.dp.lab@example.org;received=127.0.0.1:48944:1152121280
Via: SIP/2.0/TCP 127.0.0.1:49270;received=127.0.0.1:49270:12683536
Call-ID: call-id-0007.00000000000000server01.example.net
CSeq: 1 NOTIFY
From: sip:conference.wizard@local.host;pool=il-ed-aras;box=il-ed-aras-05.example-corp.com
To: sip:user.dp.lab@example.org;pool=il-ed-aras;box=il-ed-aras-05.example-corp.com
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: CallStart
X-slave-site: localhost
X-last-change: 1610843031
X-Conf_no: 9038
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.dp.lab@example.org
X-domain: DEFAULT
X-client-address: 10.1.224.108
X-From: user.dp.lab@example.org
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
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMTcuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 2
Content-Length: 1118
Content-type: application/sdp

v=0
o=user.dp.lab@example.org 245501968 104 IN IP4 il-ed-aras-01.example-corp.com
s=Denver's Lab Meeting Room
i="User%20Nine" (+14155500001x,0000000001) CallByPhone-URL
c=TN RFC2543 +14155500001x,0000000001
t=3819831831 0
m=audio 1 voice -
a=status:(2) starting
a=mediastatus:sendrecv
a=leg_no:1
a=flags:1054240
a=new_change
a=server:il-ed-aras-05.example-corp.com
a=clir:false:14155500008
a=acs_guid:000958503951
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.dp.lab@example.org
a=legCallid:1222743542@198.51.100.73
a=cdr_start_time:1610843031.087070
a=preferred_identity:14155500008
a=x-voice-callowner-login_alias:user.dp.lab@example.org
a=x-voice-legowner-login_alias:user.dp.lab@example.org
a=x-voice-alt-proj: 
a=x-voice-call_source_type:0
a=x-voice-displayname:User%20Nine
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.115:24051
a=x-voice-unique_device_id:00000008-0000-4000-8000-000000000000
a=x-voice-UF1: 
a=x-voice-coords:1.11
a=ivr:dialout
a=dialout:click_in
a=access_code:0000012
a=privs:leader
a=x-voice-smartproxy:USAZW2-LALTAIR1.example-corp.com


2021-01-17 00:24:08,032310
NOTIFY sip:user.dp.lab@example.org;pool=il-ed-aras;box=il-ed-aras-05.example-corp.com SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__user.dp.lab@example.org;received=127.0.0.1:48944:1152121280
Via: SIP/2.0/TCP 127.0.0.1:49270;received=127.0.0.1:49270:12683536
Call-ID: call-id-0007.00000000000000server01.example.net
CSeq: 3 NOTIFY
From: sip:conference.wizard@local.host;pool=il-ed-aras;box=il-ed-aras-05.example-corp.com
To: sip:user.dp.lab@example.org;pool=il-ed-aras;box=il-ed-aras-05.example-corp.com
Contact: <sip:localhost:8443;transport=ssl>
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1610843048
X-Conf_no: 9038
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.dp.lab@example.org
X-domain: DEFAULT
X-client-address: 10.1.224.108
X-From: user.dp.lab@example.org
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
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMTcuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 2
Content-Length: 1263
Content-type: application/sdp

v=0
o=user.dp.lab@example.org 245501968 115 IN IP4 il-ed-aras-01.example-corp.com
s=Denver's Lab Meeting Room
i="User%20Nine" (+14155500001x,0000000001) CallByPhone-URL
c=TN RFC2543 +14155500001x,0000000001
t=3819831847 0
m=audio 1 voice -
a=status:(203) answered
a=mediastatus:sendrecv
a=leg_no:1
a=remote:198.51.100.73:12000
a=flags:1054240
a=new_change
a=server:il-ed-aras-05.example-corp.com
a=clir:false:14155500008
a=far_end:192.0.2.72:18350
a=acs_guid:000958503951
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.dp.lab@example.org
a=legCallid:1222743542@198.51.100.73
a=audio_payload:PCMU
a=cdr_start_time:1610843031.087070
a=preferred_identity:14155500008
a=x-voice-callowner-login_alias:user.dp.lab@example.org
a=x-voice-legowner-login_alias:user.dp.lab@example.org
a=x-voice-alt-proj: 
a=x-voice-call_source_type:0
a=x-voice-displayname:User%20Nine
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.115:24051
a=x-voice-unique_device_id:00000008-0000-4000-8000-000000000000
a=x-voice-UF1: 
a=x-voice-coords:2.11
a=x-voice-coords:2.22
a=ivr:dialout
a=dialout:click_in
a=access_code:0000012
a=privs:leader
a=x-voice-smartproxy:USAZW2-LALTAIR1.example-corp.com
a=user-agent:Cisco-SIPGateway/IOS-12.x


2021-01-17 00:26:31,457767
NOTIFY sip:user.dp.lab@example.org;pool=il-ed-aras;box=il-ed-aras-05.example-corp.com SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__user.dp.lab@example.org;received=127.0.0.1:48944:1152121280
Via: SIP/2.0/TCP 127.0.0.1:49270;received=127.0.0.1:49270:12683536
Call-ID: call-id-0007.00000000000000server01.example.net
CSeq: 31 NOTIFY
From: sip:conference.wizard@local.host;pool=il-ed-aras;box=il-ed-aras-05.example-corp.com
To: sip:user.dp.lab@example.org;pool=il-ed-aras;box=il-ed-aras-05.example-corp.com
Contact: <sip:localhost:8443;transport=ssl>
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1610843191
X-Conf_no: 9038
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.dp.lab@example.org
X-domain: DEFAULT
X-client-address: 10.1.224.108
X-From: user.dp.lab@example.org
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
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMTcuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 2
Content-Length: 1318
Content-type: application/sdp

v=0
o=user.dp.lab@example.org 245501968 153 IN IP4 il-ed-aras-01.example-corp.com
s=Denver's Lab Meeting Room
i="User%20Nine" (+14155500001x,0000000001) CallByPhone-URL
c=TN RFC2543 +14155500001x,0000000001
t=3819831847 0
m=audio 1 voice -
a=status:(12) ending
a=mediastatus:sendrecv
a=cause: (30009) dropped by TNS
a=leg_no:1
a=remote:198.51.100.73:12000
a=flags:1054242
a=new_change
a=server:il-ed-aras-05.example-corp.com
a=clir:false:14155500008
a=far_end:192.0.2.72:18350
a=acs_guid:000958503951
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.dp.lab@example.org
a=legCallid:1222743542@198.51.100.73
a=audio_payload:PCMU
a=cdr_start_time:1610843031.087070
a=preferred_identity:14155500008
a=x-voice-callowner-login_alias:user.dp.lab@example.org
a=x-voice-legowner-login_alias:user.dp.lab@example.org
a=x-voice-alt-proj: 
a=x-voice-call_source_type:0
a=x-voice-displayname:User%20Nine
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.115:24051
a=x-voice-unique_device_id:00000008-0000-4000-8000-000000000000
a=x-voice-UF1: 
a=x-voice-coords:3.11
a=x-voice-coords:3.22
a=x-voice-coords:3.33
a=ivr:dialout
a=dialout:click_in
a=access_code:0000012
a=privs:leader
a=x-voice-smartproxy:USAZW2-LALTAIR1.example-corp.com
a=user-agent:Cisco-SIPGateway/IOS-12.x
