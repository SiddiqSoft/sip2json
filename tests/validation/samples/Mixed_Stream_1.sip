===========================================================================================
2020-07-02 20:27:49.124  -- SmartIOGulam() [tid:5016]
---------------------------------------------------------------------------------
SIP/2.0 200 OK
Via: SIP/2.0/TCP 127.0.0.1:62338
Call-ID: callid002000000000000000000
CSeq: 2 SUBSCRIBE
From: sip:user.jc@example.com
To: <sip:user.jc@example.com>;tag=12345678
Contact: <sip:192.0.2.37:8443;transport=ssl>;expires=3600;tag=callid002000000000000000000__sp3[USCHEQ-ASRTA01.example.net]
Expires: 360
Authorization: X-Signed start="1593721668",expire="1593725268",user="user.jc@example.com",confwiz="my string",nsadrs="il-ed-mara-01.example.net",signed="9ecb18b94f0d25fadb4cabe9d469a5fb"
User-Agent: Example eDial ACS 9.1.0b8040
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDkuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-last-change: 1593721650
X-Conf_no: 41921
X-recording-enabled: true
X-control-master: user.jc@example.com
X-subject: User Six's Meeting Room
X-restrict-notify: false
X-no-video: false
X-no-audio: false
X-suppress-system-im: false
X-Route-ID: 2
X-rollcall: disabled
X-Billing-code-required: false
X-start-muted: false
X-Call-URL: true
X-Video-SingleView: 0
X-domain: DEFAULT
Content-Length: 4947
Content-type: application/sdp

v=0
o=user.jc@example.com 1444888750 134 IN IP4 il-ed-mara-01.example.net
s=User Six's Meeting Room
i="Jim" (+15550000014) CallByPhone-URL
c=TN RFC2543 +15550000014
t=3802710347 0
m=audio 1 voice -
a=status:(203) answered  
a=mediastatus:sendrecv 
a=cdr_start_time:1593721508.0
a=leg_no:1
a=remote:192.0.2.36:12058
a=flags:1050162
a=primary:true
a=server:il-ed-mara-06.example.net
a=clir:false:3125550011
a=far_end:192.0.2.73:41488
a=acs_guid:001367321370
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.jc@example.com
a=legCallid:646397542@192.0.2.36
a=audio_payload:PCMU
a=preferred_identity:3125550011
a=x-voice-callowner-login_alias:user.jc@example.com
a=x-voice-legowner-login_alias:user.jc@example.com
a=x-voice-call_source_type:0
a=x-voice-displayname:Jim
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.222:39941
a=x-voice-unique_device_id:00000004-0000-4000-8000-000000000000
a=x-voice-UF1:None
a=ivr:dialout-confirm
a=access_code:0000015
a=privs:participant
a=dialout:click_in
a=x-voice-LICENSE_URI:urn:object:r2license:user:com.example.consumer:7BF150F5-5230-4946-B3DA-66337993A01B
a=x-voice-smartproxy:USCHEQ-ASRTA01.example.net
a=user-agent:Cisco-SIPGateway/IOS-16.3.3
v=0
o=user.jc@example.com 1444923171 132 IN IP4 il-ed-mara-01.example.net
s=User Six's Meeting Room
i="User%20Ten" (+15550000014) CallByPhone-URL
c=TN RFC2543 +15550000014
t=3802710356 0
m=audio 1 voice -
a=status:(203) answered  
a=mediastatus:sendrecv
a=cdr_start_time:1593721542.0
a=leg_no:2
a=remote:192.0.2.36:12114
a=flags:1050146
a=server:il-ed-mara-06.example.net
a=clir:false:3125550011
a=far_end:192.0.2.73:27276
a=acs_guid:001367321370
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.jc@example.com
a=legCallid:646397548@192.0.2.36
a=audio_payload:PCMU
a=preferred_identity:3125550011
a=x-voice-callowner-login_alias:user.jc@example.com
a=x-voice-legowner-login_alias:user.jc@example.com
a=x-voice-call_source_type:0
a=x-voice-displayname:User%20Ten
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.222:681
a=x-voice-unique_device_id:00000009-0000-4000-8000-000000000000
a=x-voice-UF1:None
a=ivr:dialout-confirm
a=access_code:0000015
a=privs:participant
a=dialout:click_in
a=x-voice-LICENSE_URI:urn:object:r2license:user:com.example.consumer:7BF150F5-5230-4946-B3DA-66337993A01B
a=x-voice-smartproxy:USCHEQ-ASRTA01.example.net
a=user-agent:Cisco-SIPGateway/IOS-16.3.3
v=0
o=user.jc@example.com 1445019609 126 IN IP4 il-ed-mara-01.example.net
s=User Six's Meeting Room
i="Tanya" (+15550000014) CallByPhone-URL
c=TN RFC2543 +15550000014
t=3802710447 0
m=audio 1 voice -
a=status:(203) answered  
a=mediastatus:sendrecv
a=cdr_start_time:1593721638.0
a=leg_no:4
a=remote:192.0.2.36:12154
a=flags:1050146
a=server:il-ed-mara-06.example.net
a=clir:false:3125550011
a=far_end:192.0.2.73:40694
a=acs_guid:001367321370
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.jc@example.com
a=legCallid:646397570@192.0.2.36
a=audio_payload:PCMU
a=preferred_identity:3125550011
a=x-voice-callowner-login_alias:user.jc@example.com
a=x-voice-legowner-login_alias:user.jc@example.com
a=x-voice-call_source_type:0
a=x-voice-displayname:Tanya
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.222:51156
a=x-voice-unique_device_id:00000003-0000-4000-8000-000000000000
a=x-voice-UF1:None
a=ivr:dialout-confirm
a=access_code:0000015
a=privs:participant
a=dialout:click_in
a=x-voice-LICENSE_URI:urn:object:r2license:user:com.example.consumer:7BF150F5-5230-4946-B3DA-66337993A01B
a=x-voice-smartproxy:USCHEQ-ASRTA01.example.net
a=user-agent:Cisco-SIPGateway/IOS-16.3.3
v=0
o=user.jc@example.com 1445022703 126 IN IP4 il-ed-mara-01.example.net
s=User Six's Meeting Room
i="User%20Fifteen" (+15550000014) CallByPhone-URL
c=TN RFC2543 +15550000014
t=3802710450 0
m=audio 1 voice -
a=status:(203) answered  
a=mediastatus:sendrecv
a=cdr_start_time:1593721642.0
a=leg_no:5
a=remote:192.0.2.36:12174
a=flags:1050146
a=server:il-ed-mara-06.example.net
a=clir:false:3125550011
a=far_end:192.0.2.73:40564
a=acs_guid:001367321370
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.jc@example.com
a=legCallid:646397572@192.0.2.36
a=audio_payload:PCMU
a=preferred_identity:3125550011
a=x-voice-callowner-login_alias:user.jc@example.com
a=x-voice-legowner-login_alias:user.jc@example.com
a=x-voice-call_source_type:0
a=x-voice-displayname:User%20Fifteen
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.222:49786
a=x-voice-unique_device_id:0000000a-0000-4000-8000-000000000000
a=x-voice-UF1:None
a=ivr:dialout-confirm
a=access_code:0000015
a=privs:participant
a=dialout:click_in
a=x-voice-LICENSE_URI:urn:object:r2license:user:com.example.consumer:7BF150F5-5230-4946-B3DA-66337993A01B
a=x-voice-smartproxy:USCHEQ-ASRTA01.example.net
a=user-agent:Cisco-SIPGateway/IOS-16.3.3
NOTIFY sip:user.jc@example.com SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Call-ID: callid002000000000000000000
CSeq: 1 NOTIFY
From: sip:user.jc@example.com;tag=12345678
To: sip:user.jc@example.com
Contact: <sip:192.0.2.37:8443;transport=ssl>;expires=3600;tag=callid002000000000000000000__sp3[USCHEQ-ASRTA01.example.net]
Expires: 360
Authorization: X-Signed start="1593721668",expire="1593725268",user="user.jc@example.com",confwiz="my string",nsadrs="il-ed-mara-01.example.net",signed="9ecb18b94f0d25fadb4cabe9d469a5fb"
User-Agent: Example eDial ACS 9.1.0b8040
X-NO-TNS: NOTIFY
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDkuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-last-change: 1593721650
X-Conf_no: 41921
X-recording-enabled: true
X-control-master: user.jc@example.com
X-subject: User Six's Meeting Room
X-restrict-notify: false
X-no-video: false
X-no-audio: false
X-suppress-system-im: false
X-Route-ID: 2
X-rollcall: disabled
X-Billing-code-required: false
X-start-muted: false
X-Call-URL: true
X-Video-SingleView: 0
X-domain: DEFAULT
Content-Length: 4947
Content-type: application/sdp

v=0
o=user.jc@example.com 1444888750 134 IN IP4 il-ed-mara-01.example.net
s=User Six's Meeting Room
i="Jim" (+15550000014) CallByPhone-URL
c=TN RFC2543 +15550000014
t=3802710347 0
m=audio 1 voice -
a=status:(203) answered  
a=mediastatus:sendrecv 
a=cdr_start_time:1593721508.0
a=leg_no:1
a=remote:192.0.2.36:12058
a=flags:1050162
a=primary:true
a=server:il-ed-mara-06.example.net
a=clir:false:3125550011
a=far_end:192.0.2.73:41488
a=acs_guid:001367321370
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.jc@example.com
a=legCallid:646397542@192.0.2.36
a=audio_payload:PCMU
a=preferred_identity:3125550011
a=x-voice-callowner-login_alias:user.jc@example.com
a=x-voice-legowner-login_alias:user.jc@example.com
a=x-voice-call_source_type:0
a=x-voice-displayname:Jim
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.222:39941
a=x-voice-unique_device_id:00000004-0000-4000-8000-000000000000
a=x-voice-UF1:None
a=ivr:dialout-confirm
a=access_code:0000015
a=privs:participant
a=dialout:click_in
a=x-voice-LICENSE_URI:urn:object:r2license:user:com.example.consumer:7BF150F5-5230-4946-B3DA-66337993A01B
a=x-voice-smartproxy:USCHEQ-ASRTA01.example.net
a=user-agent:Cisco-SIPGateway/IOS-16.3.3
v=0
o=user.jc@example.com 1444923171 132 IN IP4 il-ed-mara-01.example.net
s=User Six's Meeting Room
i="User%20Ten" (+15550000014) CallByPhone-URL
c=TN RFC2543 +15550000014
t=3802710356 0
m=audio 1 voice -
a=status:(203) answered  
a=mediastatus:sendrecv
a=cdr_start_time:1593721542.0
a=leg_no:2
a=remote:192.0.2.36:12114
a=flags:1050146
a=server:il-ed-mara-06.example.net
a=clir:false:3125550011
a=far_end:192.0.2.73:27276
a=acs_guid:001367321370
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.jc@example.com
a=legCallid:646397548@192.0.2.36
a=audio_payload:PCMU
a=preferred_identity:3125550011
a=x-voice-callowner-login_alias:user.jc@example.com
a=x-voice-legowner-login_alias:user.jc@example.com
a=x-voice-call_source_type:0
a=x-voice-displayname:User%20Ten
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.222:681
a=x-voice-unique_device_id:00000009-0000-4000-8000-000000000000
a=x-voice-UF1:None
a=ivr:dialout-confirm
a=access_code:0000015
a=privs:participant
a=dialout:click_in
a=x-voice-LICENSE_URI:urn:object:r2license:user:com.example.consumer:7BF150F5-5230-4946-B3DA-66337993A01B
a=x-voice-smartproxy:USCHEQ-ASRTA01.example.net
a=user-agent:Cisco-SIPGateway/IOS-16.3.3
v=0
o=user.jc@example.com 1445019609 126 IN IP4 il-ed-mara-01.example.net
s=User Six's Meeting Room
i="Tanya" (+15550000014) CallByPhone-URL
c=TN RFC2543 +15550000014
t=3802710447 0
m=audio 1 voice -
a=status:(203) answered  
a=mediastatus:sendrecv
a=cdr_start_time:1593721638.0
a=leg_no:4
a=remote:192.0.2.36:12154
a=flags:1050146
a=server:il-ed-mara-06.example.net
a=clir:false:3125550011
a=far_end:192.0.2.73:40694
a=acs_guid:001367321370
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.jc@example.com
a=legCallid:646397570@192.0.2.36
a=audio_payload:PCMU
a=preferred_identity:3125550011
a=x-voice-callowner-login_alias:user.jc@example.com
a=x-voice-legowner-login_alias:user.jc@example.com
a=x-voice-call_source_type:0
a=x-voice-displayname:Tanya
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.222:51156
a=x-voice-unique_device_id:00000003-0000-4000-8000-000000000000
a=x-voice-UF1:None
a=ivr:dialout-confirm
a=access_code:0000015
a=privs:participant
a=dialout:click_in
a=x-voice-LICENSE_URI:urn:object:r2license:user:com.example.consumer:7BF150F5-5230-4946-B3DA-66337993A01B
a=x-voice-smartproxy:USCHEQ-ASRTA01.example.net
a=user-agent:Cisco-SIPGateway/IOS-16.3.3
v=0
o=user.jc@example.com 1445022703 126 IN IP4 il-ed-mara-01.example.net
s=User Six's Meeting Room
i="User%20Fifteen" (+15550000014) CallByPhone-URL
c=TN RFC2543 +15550000014
t=3802710450 0
m=audio 1 voice -
a=status:(203) answered  
a=mediastatus:sendrecv
a=cdr_start_time:1593721642.0
a=leg_no:5
a=remote:192.0.2.36:12174
a=flags:1050146
a=server:il-ed-mara-06.example.net
a=clir:false:3125550011
a=far_end:192.0.2.73:40564
a=acs_guid:001367321370
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.jc@example.com
a=legCallid:646397572@192.0.2.36
a=audio_payload:PCMU
a=preferred_identity:3125550011
a=x-voice-callowner-login_alias:user.jc@example.com
a=x-voice-legowner-login_alias:user.jc@example.com
a=x-voice-call_source_type:0
a=x-voice-displayname:User%20Fifteen
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.222:49786
a=x-voice-unique_device_id:0000000a-0000-4000-8000-000000000000
a=x-voice-UF1:None
a=ivr:dialout-confirm
a=access_code:0000015
a=privs:participant
a=dialout:click_in
a=x-voice-LICENSE_URI:urn:object:r2license:user:com.example.consumer:7BF150F5-5230-4946-B3DA-66337993A01B
a=x-voice-smartproxy:USCHEQ-ASRTA01.example.net
a=user-agent:Cisco-SIPGateway/IOS-16.3.3
===========================================================================================
2020-07-02 20:27:49.639  -- SmartIOGulam() [tid:5016]
---------------------------------------------------------------------------------
SIP/2.0 100 Trying
Via: SIP/2.0/TCP 127.0.0.1:62338
Call-ID: callid002000000000000000000
CSeq: 3 INVITE
Subject: User Six's Meeting Room
From: sip:user.jc@example.com
To: sip:user.jc@example.com
Contact: <sip:192.0.2.37:8443;transport=ssl>;expires=3600;tag=callid002000000000000000000__sp3[USCHEQ-ASRTA01.example.net]
User-Agent: Example eDial ACS 9.1.0b8040
X-Message-Time: 1593721670540996
X-Route-ID: 2
X-recording-enabled: true
X-client-address: 192.0.2.163
X-domain: DEFAULT
uthorization: X-Signed start="1593721669",expire="1593725269",user="user.jc@example.com",confwiz="my string",nsadrs="il-ed-mara-01.example.net",signed="a73789748d9c7dd2d1092794597d2a57"
X-Auto-Record: false
X-Billing-code-required: false
X-dialout: allowed
X-project-code: None
X-subject: User Six's Meeting Room
x-voice-callowner-login_alias: user.jc@example.com
x-voice-callsource: 0
Content-Length: 0

===========================================================================================
2020-07-02 20:27:49.764  -- SmartIOGulam() [tid:5016]
---------------------------------------------------------------------------------
NOTIFY sip:user.jc@example.com SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/tcp localhost:8443;branch=user.jc@example.com__eDial_sep__user.jc@example.com
Call-ID: callid002000000000000000000
CSeq: 32 NOTIFY
From: sip:user.jc@example.com;pool=il-ed-mara;box=il-ed-mara-06.example.net;tag=12345678
To: "jcollier" <sip:user.jc@example.com>
Contact: <sip:localhost:8443;transport=ssl>
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1593721670
X-Conf_no: 41921
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.jc@example.com
X-domain: DEFAULT
X-client-address: 192.0.2.163
X-From: user.jc@example.com
X-legs-on-server: 197
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
X-subject: User Six's Meeting Room
X-project-code: None
X-department-code: retail:USA
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDkuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 2
X-Call-Start-Time: 1593721508.411058
Content-Length: 1117
Content-type: application/sdp

v=0
o=user.jc@example.com 1445051234 104 IN IP4 il-ed-mara-01.example.net
s=User Six's Meeting Room
i="User%20Six" (+15550000014) CallByPhone-URL
c=TN RFC2543 +15550000014
t=3802710470 0
m=audio 1 voice -
a=status:(2) starting
a=mediastatus:sendrecv
a=leg_no:6
a=flags:1054240
a=new_change
a=server:il-ed-mara-06.example.net
a=clir:false:3125550011
a=acs_guid:001367321370
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.jc@example.com
a=legCallid:646397585@192.0.2.36
a=preferred_identity:3125550011
a=x-voice-callowner-login_alias:user.jc@example.com
a=x-voice-legowner-login_alias:user.jc@example.com
a=x-voice-alt-proj:None
a=x-voice-call_source_type:0
a=x-voice-displayname:User%20Six
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.222:14733
a=x-voice-unique_device_id:00000001-0000-4000-8000-000000000000
a=x-voice-UF1:None
a=ivr:dialout-confirm
a=dialout:click_in
a=access_code:0000010
a=privs:leader
a=x-voice-LICENSE_URI:urn:object:r2license:user:com.example.consumer:7BF150F5-5230-4946-B3DA-66337993A01B
a=x-voice-smartproxy:USCHEQ-ASRTA01.example.net
===========================================================================================
2020-07-02 20:27:51.577  -- SmartIOGulam() [tid:5016]
---------------------------------------------------------------------------------
NOTIFY sip:user.jc@example.com SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/tcp localhost:8443;branch=user.jc@example.com__eDial_sep__user.jc@example.com
Call-ID: callid002000000000000000000
CSeq: 33 NOTIFY
From: sip:user.jc@example.com;pool=il-ed-mara;box=il-ed-mara-06.example.net;tag=12345678
To: "jcollier" <sip:user.jc@example.com>
Contact: <sip:localhost:8443;transport=ssl>
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1593721670
X-Conf_no: 41921
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.jc@example.com
X-domain: DEFAULT
X-client-address: 192.0.2.163
X-From: user.jc@example.com
X-legs-on-server: 197
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
X-subject: User Six's Meeting Room
X-project-code: None
X-department-code: retail:USA
X-active-talker: 
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDkuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 2
X-Call-Start-Time: 1593721508.411058
Content-Length: 1260
Content-type: application/sdp

v=0
o=user.jc@example.com 1444888750 135 IN IP4 il-ed-mara-01.example.net
s=User Six's Meeting Room
i="Jim" (+15550000014) CallByPhone-URL
c=TN RFC2543 +15550000014
t=3802710347 0
m=audio 1 voice -
a=status:(203) answered  
a=mediastatus:sendrecv active
a=cdr_start_time:1593721508.0
a=leg_no:1
a=remote:192.0.2.36:12058
a=flags:1050162
a=new_change
a=primary:true
a=server:il-ed-mara-06.example.net
a=clir:false:3125550011
a=far_end:192.0.2.73:41488
a=acs_guid:001367321370
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.jc@example.com
a=legCallid:646397542@192.0.2.36
a=audio_payload:PCMU
a=preferred_identity:3125550011
a=x-voice-callowner-login_alias:user.jc@example.com
a=x-voice-legowner-login_alias:user.jc@example.com
a=x-voice-call_source_type:0
a=x-voice-displayname:Jim
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.222:39941
a=x-voice-unique_device_id:00000004-0000-4000-8000-000000000000
a=x-voice-UF1:None
a=ivr:dialout-confirm
a=access_code:0000015
a=privs:participant
a=dialout:click_in
a=x-voice-LICENSE_URI:urn:object:r2license:user:com.example.consumer:7BF150F5-5230-4946-B3DA-66337993A01B
a=x-voice-smartproxy:USCHEQ-ASRTA01.example.net
a=user-agent:Cisco-SIPGateway/IOS-16.3.3
===========================================================================================
2020-07-02 20:27:56.264  -- SmartIOGulam() [tid:5016]
---------------------------------------------------------------------------------
NOTIFY sip:user.jc@example.com SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/tcp localhost:8443;branch=user.jc@example.com__eDial_sep__user.jc@example.com
Call-ID: callid002000000000000000000
CSeq: 34 NOTIFY
From: sip:user.jc@example.com;pool=il-ed-mara;box=il-ed-mara-06.example.net;tag=12345678
To: "jcollier" <sip:user.jc@example.com>
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: LegAdd
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1593721677
X-Conf_no: 41921
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.jc@example.com
X-domain: DEFAULT
X-client-address: 192.0.2.163
X-From: user.jc@example.com
X-legs-on-server: 196
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
X-subject: User Six's Meeting Room
X-project-code: None
X-department-code: retail:USA
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDkuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 2
X-Call-Start-Time: 1593721508.411058
Content-Length: 1271
Content-type: application/sdp

v=0
o=user.jc@example.com 1445051234 115 IN IP4 il-ed-mara-01.example.net
s=User Six's Meeting Room
i="User%20Six" (+15550000014) CallByPhone-URL
c=TN RFC2543 +15550000014
t=3802710477 0
m=audio 1 voice -
a=status:(203) answered  
a=mediastatus:sendrecv
a=cdr_start_time:1593721670.0
a=leg_no:6
a=remote:192.0.2.36:12208
a=flags:1054240
a=new_change
a=server:il-ed-mara-06.example.net
a=clir:false:3125550011
a=far_end:192.0.2.73:23744
a=acs_guid:001367321370
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.jc@example.com
a=legCallid:646397585@192.0.2.36
a=audio_payload:PCMU
a=preferred_identity:3125550011
a=x-voice-callowner-login_alias:user.jc@example.com
a=x-voice-legowner-login_alias:user.jc@example.com
a=x-voice-alt-proj:None
a=x-voice-call_source_type:0
a=x-voice-displayname:User%20Six
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.222:14733
a=x-voice-unique_device_id:00000001-0000-4000-8000-000000000000
a=x-voice-UF1:None
a=ivr:dialout-confirm
a=dialout:click_in
a=access_code:0000010
a=privs:leader
a=x-voice-LICENSE_URI:urn:object:r2license:user:com.example.consumer:7BF150F5-5230-4946-B3DA-66337993A01B
a=x-voice-smartproxy:USCHEQ-ASRTA01.example.net
a=user-agent:Cisco-SIPGateway/IOS-16.3.3
NOTIFY sip:user.jc@example.com SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/tcp localhost:8443;branch=user.jc@example.com__eDial_sep__user.jc@example.com
Call-ID: callid002000000000000000000
CSeq: 35 NOTIFY
From: sip:user.jc@example.com;pool=il-ed-mara;box=il-ed-mara-06.example.net;tag=12345678
To: "jcollier" <sip:user.jc@example.com>
Contact: <sip:localhost:8443;transport=ssl>
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1593721677
X-Conf_no: 41921
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.jc@example.com
X-domain: DEFAULT
X-client-address: 192.0.2.163
X-From: user.jc@example.com
X-legs-on-server: 196
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
X-subject: User Six's Meeting Room
X-project-code: None
X-department-code: retail:USA
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDkuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 2
X-Call-Start-Time: 1593721508.411058
Content-Length: 1271
Content-type: application/sdp

v=0
o=user.jc@example.com 1445051234 126 IN IP4 il-ed-mara-01.example.net
s=User Six's Meeting Room
i="User%20Six" (+15550000014) CallByPhone-URL
c=TN RFC2543 +15550000014
t=3802710477 0
m=audio 1 voice -
a=status:(203) answered  
a=mediastatus:sendrecv
a=cdr_start_time:1593721670.0
a=leg_no:6
a=remote:192.0.2.36:12208
a=flags:1054242
a=new_change
a=server:il-ed-mara-06.example.net
a=clir:false:3125550011
a=far_end:192.0.2.73:23744
a=acs_guid:001367321370
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.jc@example.com
a=legCallid:646397585@192.0.2.36
a=audio_payload:PCMU
a=preferred_identity:3125550011
a=x-voice-callowner-login_alias:user.jc@example.com
a=x-voice-legowner-login_alias:user.jc@example.com
a=x-voice-alt-proj:None
a=x-voice-call_source_type:0
a=x-voice-displayname:User%20Six
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.222:14733
a=x-voice-unique_device_id:00000001-0000-4000-8000-000000000000
a=x-voice-UF1:None
a=ivr:dialout-confirm
a=dialout:click_in
a=access_code:0000010
a=privs:leader
a=x-voice-LICENSE_URI:urn:object:r2license:user:com.example.consumer:7BF150F5-5230-4946-B3DA-66337993A01B
a=x-voice-smartproxy:USCHEQ-ASRTA01.example.net
a=user-agent:Cisco-SIPGateway/IOS-16.3.3
===========================================================================================
2020-07-02 20:27:58.327  -- SmartIOGulam() [tid:5016]
---------------------------------------------------------------------------------
NOTIFY sip:user.jc@example.com SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/tcp localhost:8443;branch=user.jc@example.com__eDial_sep__user.jc@example.com
Call-ID: callid002000000000000000000
CSeq: 36 NOTIFY
From: sip:user.jc@example.com;pool=il-ed-mara;box=il-ed-mara-06.example.net;tag=12345678
To: "jcollier" <sip:user.jc@example.com>
Contact: <sip:localhost:8443;transport=ssl>
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1593721677
X-Conf_no: 41921
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.jc@example.com
X-domain: DEFAULT
X-client-address: 192.0.2.163
X-From: user.jc@example.com
X-legs-on-server: 196
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
X-subject: User Six's Meeting Room
X-project-code: None
X-department-code: retail:USA
X-active-talker: 
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDkuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 2
X-Call-Start-Time: 1593721508.411058
Content-Length: 1253
Content-type: application/sdp

v=0
o=user.jc@example.com 1444888750 136 IN IP4 il-ed-mara-01.example.net
s=User Six's Meeting Room
i="Jim" (+15550000014) CallByPhone-URL
c=TN RFC2543 +15550000014
t=3802710347 0
m=audio 1 voice -
a=status:(203) answered  
a=mediastatus:sendrecv
a=cdr_start_time:1593721508.0
a=leg_no:1
a=remote:192.0.2.36:12058
a=flags:1050162
a=new_change
a=primary:true
a=server:il-ed-mara-06.example.net
a=clir:false:3125550011
a=far_end:192.0.2.73:41488
a=acs_guid:001367321370
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.jc@example.com
a=legCallid:646397542@192.0.2.36
a=audio_payload:PCMU
a=preferred_identity:3125550011
a=x-voice-callowner-login_alias:user.jc@example.com
a=x-voice-legowner-login_alias:user.jc@example.com
a=x-voice-call_source_type:0
a=x-voice-displayname:Jim
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.222:39941
a=x-voice-unique_device_id:00000004-0000-4000-8000-000000000000
a=x-voice-UF1:None
a=ivr:dialout-confirm
a=access_code:0000015
a=privs:participant
a=dialout:click_in
a=x-voice-LICENSE_URI:urn:object:r2license:user:com.example.consumer:7BF150F5-5230-4946-B3DA-66337993A01B
a=x-voice-smartproxy:USCHEQ-ASRTA01.example.net
a=user-agent:Cisco-SIPGateway/IOS-16.3.3
===========================================================================================
2020-07-02 20:28:02.452  -- SmartIOGulam() [tid:5016]
---------------------------------------------------------------------------------
NOTIFY sip:user.jc@example.com SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/tcp localhost:8443;branch=user.jc@example.com__eDial_sep__user.jc@example.com
Call-ID: callid002000000000000000000
CSeq: 37 NOTIFY
From: sip:user.jc@example.com;pool=il-ed-mara;box=il-ed-mara-06.example.net;tag=12345678
To: "jcollier" <sip:user.jc@example.com>
Contact: <sip:localhost:8443;transport=ssl>
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1593721677
X-Conf_no: 41921
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.jc@example.com
X-domain: DEFAULT
X-client-address: 192.0.2.163
X-From: user.jc@example.com
X-legs-on-server: 197
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
X-subject: User Six's Meeting Room
X-project-code: None
X-department-code: retail:USA
X-active-talker: 
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDkuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 2
X-Call-Start-Time: 1593721508.411058
Content-Length: 1260
Content-type: application/sdp

v=0
o=user.jc@example.com 1444888750 137 IN IP4 il-ed-mara-01.example.net
s=User Six's Meeting Room
i="Jim" (+15550000014) CallByPhone-URL
c=TN RFC2543 +15550000014
t=3802710347 0
m=audio 1 voice -
a=status:(203) answered  
a=mediastatus:sendrecv active
a=cdr_start_time:1593721508.0
a=leg_no:1
a=remote:192.0.2.36:12058
a=flags:1050162
a=new_change
a=primary:true
a=server:il-ed-mara-06.example.net
a=clir:false:3125550011
a=far_end:192.0.2.73:41488
a=acs_guid:001367321370
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.jc@example.com
a=legCallid:646397542@192.0.2.36
a=audio_payload:PCMU
a=preferred_identity:3125550011
a=x-voice-callowner-login_alias:user.jc@example.com
a=x-voice-legowner-login_alias:user.jc@example.com
a=x-voice-call_source_type:0
a=x-voice-displayname:Jim
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.222:39941
a=x-voice-unique_device_id:00000004-0000-4000-8000-000000000000
a=x-voice-UF1:None
a=ivr:dialout-confirm
a=access_code:0000015
a=privs:participant
a=dialout:click_in
a=x-voice-LICENSE_URI:urn:object:r2license:user:com.example.consumer:7BF150F5-5230-4946-B3DA-66337993A01B
a=x-voice-smartproxy:USCHEQ-ASRTA01.example.net
a=user-agent:Cisco-SIPGateway/IOS-16.3.3
===========================================================================================
2020-07-02 20:28:03.327  -- SmartIOGulam() [tid:5016]
---------------------------------------------------------------------------------
SIP/2.0 100 Trying
Via: SIP/2.0/TCP 127.0.0.1:62338
Call-ID: callid002000000000000000000
CSeq: 4 INVITE
Subject: User Six's Meeting Room
From: sip:user.jc@example.com
To: sip:user.jc@example.com
Contact: <sip:192.0.2.37:8443;transport=ssl>;expires=3600;tag=callid002000000000000000000__sp3[USCHEQ-ASRTA01.example.net]
User-Agent: Example eDial ACS 9.1.0b8040
X-Message-Time: 1593721684228957
X-Route-ID: 2
X-recording-enabled: true
X-client-address: 192.0.2.163
X-domain: DEFAULT
uthorization: X-Signed start="1593721683",expire="1593725283",user="user.jc@example.com",confwiz="my string",nsadrs="il-ed-mara-01.example.net",signed="3dadfd7ef8b65962e440984dd3a8cfb8"
X-Auto-Record: false
X-Billing-code-required: false
X-dialout: allowed
X-project-code: None
X-subject: User Six's Meeting Room
x-voice-callowner-login_alias: user.jc@example.com
x-voice-callsource: 0
Content-Length: 0

===========================================================================================
2020-07-02 20:28:03.515  -- SmartIOGulam() [tid:5016]
---------------------------------------------------------------------------------
NOTIFY sip:user.jc@example.com SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/tcp localhost:8443;branch=user.jc@example.com__eDial_sep__user.jc@example.com
Call-ID: callid002000000000000000000
CSeq: 38 NOTIFY
From: sip:user.jc@example.com;pool=il-ed-mara;box=il-ed-mara-06.example.net;tag=12345678
To: "jcollier" <sip:user.jc@example.com>
Contact: <sip:localhost:8443;transport=ssl>
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1593721684
X-Conf_no: 41921
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.jc@example.com
X-domain: DEFAULT
X-client-address: 192.0.2.163
X-From: user.jc@example.com
X-legs-on-server: 198
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
X-subject: User Six's Meeting Room
X-project-code: None
X-department-code: retail:USA
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDkuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 2
X-Call-Start-Time: 1593721508.411058
Content-Length: 1103
Content-type: application/sdp

v=0
o=user.jc@example.com 1445064890 104 IN IP4 il-ed-mara-01.example.net
s=User Six's Meeting Room
i="User%20Twelve" (+15550000014) CallByPhone-URL
c=TN RFC2543 +15550000014
t=3802710484 0
m=audio 1 voice -
a=status:(2) starting
a=mediastatus:sendrecv
a=leg_no:7
a=flags:1050144
a=new_change
a=server:il-ed-mara-06.example.net
a=clir:false:3125550011
a=acs_guid:001367321370
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.jc@example.com
a=legCallid:646397587@192.0.2.36
a=preferred_identity:3125550011
a=x-voice-callowner-login_alias:user.jc@example.com
a=x-voice-legowner-login_alias:user.jc@example.com
a=x-voice-call_source_type:0
a=x-voice-displayname:User%20Twelve
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.222:53027
a=x-voice-unique_device_id:0000000c-0000-4000-8000-000000000000
a=x-voice-UF1:None
a=ivr:dialout-confirm
a=access_code:0000015
a=privs:participant
a=dialout:click_in
a=x-voice-LICENSE_URI:urn:object:r2license:user:com.example.consumer:7BF150F5-5230-4946-B3DA-66337993A01B
a=x-voice-smartproxy:USCHEQ-ASRTA01.example.net
===========================================================================================
2020-07-02 20:28:12.327  -- SmartIOGulam() [tid:5016]
---------------------------------------------------------------------------------
NOTIFY sip:user.jc@example.com SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/tcp localhost:8443;branch=user.jc@example.com__eDial_sep__user.jc@example.com
Call-ID: callid002000000000000000000
CSeq: 39 NOTIFY
From: sip:user.jc@example.com;pool=il-ed-mara;box=il-ed-mara-06.example.net;tag=12345678
To: "jcollier" <sip:user.jc@example.com>
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: LegAdd
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1593721693
X-Conf_no: 41921
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.jc@example.com
X-domain: DEFAULT
X-client-address: 192.0.2.163
X-From: user.jc@example.com
X-legs-on-server: 201
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
X-subject: User Six's Meeting Room
X-project-code: None
X-department-code: retail:USA
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDkuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 2
X-Call-Start-Time: 1593721508.411058
Content-Length: 1257
Content-type: application/sdp

v=0
o=user.jc@example.com 1445064890 115 IN IP4 il-ed-mara-01.example.net
s=User Six's Meeting Room
i="User%20Twelve" (+15550000014) CallByPhone-URL
c=TN RFC2543 +15550000014
t=3802710493 0
m=audio 1 voice -
a=status:(203) answered  
a=mediastatus:sendrecv
a=cdr_start_time:1593721684.0
a=leg_no:7
a=remote:192.0.2.36:12210
a=flags:1050144
a=new_change
a=server:il-ed-mara-06.example.net
a=clir:false:3125550011
a=far_end:192.0.2.73:42902
a=acs_guid:001367321370
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.jc@example.com
a=legCallid:646397587@192.0.2.36
a=audio_payload:PCMU
a=preferred_identity:3125550011
a=x-voice-callowner-login_alias:user.jc@example.com
a=x-voice-legowner-login_alias:user.jc@example.com
a=x-voice-call_source_type:0
a=x-voice-displayname:User%20Twelve
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.222:53027
a=x-voice-unique_device_id:0000000c-0000-4000-8000-000000000000
a=x-voice-UF1:None
a=ivr:dialout-confirm
a=access_code:0000015
a=privs:participant
a=dialout:click_in
a=x-voice-LICENSE_URI:urn:object:r2license:user:com.example.consumer:7BF150F5-5230-4946-B3DA-66337993A01B
a=x-voice-smartproxy:USCHEQ-ASRTA01.example.net
a=user-agent:Cisco-SIPGateway/IOS-16.3.3
NOTIFY sip:user.jc@example.com SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/tcp localhost:8443;branch=user.jc@example.com__eDial_sep__user.jc@example.com
Call-ID: callid002000000000000000000
CSeq: 40 NOTIFY
From: sip:user.jc@example.com;pool=il-ed-mara;box=il-ed-mara-06.example.net;tag=12345678
To: "jcollier" <sip:user.jc@example.com>
Contact: <sip:localhost:8443;transport=ssl>
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1593721693
X-Conf_no: 41921
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.jc@example.com
X-domain: DEFAULT
X-client-address: 192.0.2.163
X-From: user.jc@example.com
X-legs-on-server: 201
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
X-subject: User Six's Meeting Room
X-project-code: None
X-department-code: retail:USA
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDkuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 2
X-Call-Start-Time: 1593721508.411058
Content-Length: 1257
Content-type: application/sdp

v=0
o=user.jc@example.com 1445064890 126 IN IP4 il-ed-mara-01.example.net
s=User Six's Meeting Room
i="User%20Twelve" (+15550000014) CallByPhone-URL
c=TN RFC2543 +15550000014
t=3802710493 0
m=audio 1 voice -
a=status:(203) answered  
a=mediastatus:sendrecv
a=cdr_start_time:1593721684.0
a=leg_no:7
a=remote:192.0.2.36:12210
a=flags:1050146
a=new_change
a=server:il-ed-mara-06.example.net
a=clir:false:3125550011
a=far_end:192.0.2.73:42902
a=acs_guid:001367321370
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.jc@example.com
a=legCallid:646397587@192.0.2.36
a=audio_payload:PCMU
a=preferred_identity:3125550011
a=x-voice-callowner-login_alias:user.jc@example.com
a=x-voice-legowner-login_alias:user.jc@example.com
a=x-voice-call_source_type:0
a=x-voice-displayname:User%20Twelve
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.222:53027
a=x-voice-unique_device_id:0000000c-0000-4000-8000-000000000000
a=x-voice-UF1:None
a=ivr:dialout-confirm
a=access_code:0000015
a=privs:participant
a=dialout:click_in
a=x-voice-LICENSE_URI:urn:object:r2license:user:com.example.consumer:7BF150F5-5230-4946-B3DA-66337993A01B
a=x-voice-smartproxy:USCHEQ-ASRTA01.example.net
a=user-agent:Cisco-SIPGateway/IOS-16.3.3
===========================================================================================
2020-07-02 20:28:22.093  -- SmartIOGulam() [tid:5016]
---------------------------------------------------------------------------------
NOTIFY sip:user.jc@example.com SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/tcp localhost:8443;branch=user.jc@example.com__eDial_sep__user.jc@example.com
Call-ID: callid002000000000000000000
CSeq: 41 NOTIFY
From: sip:user.jc@example.com;pool=il-ed-mara;box=il-ed-mara-06.example.net;tag=12345678
To: "jcollier" <sip:user.jc@example.com>
Contact: <sip:localhost:8443;transport=ssl>
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1593721693
X-Conf_no: 41921
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.jc@example.com
X-domain: DEFAULT
X-client-address: 192.0.2.163
X-From: user.jc@example.com
X-legs-on-server: 204
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
X-subject: User Six's Meeting Room
X-project-code: None
X-department-code: retail:USA
X-active-talker: 
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDkuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 2
X-Call-Start-Time: 1593721508.411058
Content-Length: 1260
Content-type: application/sdp

v=0
o=user.jc@example.com 1444888750 138 IN IP4 il-ed-mara-01.example.net
s=User Six's Meeting Room
i="Jim" (+15550000014) CallByPhone-URL
c=TN RFC2543 +15550000014
t=3802710347 0
m=audio 1 voice -
a=status:(203) answered  
a=mediastatus:sendrecv active
a=cdr_start_time:1593721508.0
a=leg_no:1
a=remote:192.0.2.36:12058
a=flags:1050162
a=new_change
a=primary:true
a=server:il-ed-mara-06.example.net
a=clir:false:3125550011
a=far_end:192.0.2.73:41488
a=acs_guid:001367321370
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.jc@example.com
a=legCallid:646397542@192.0.2.36
a=audio_payload:PCMU
a=preferred_identity:3125550011
a=x-voice-callowner-login_alias:user.jc@example.com
a=x-voice-legowner-login_alias:user.jc@example.com
a=x-voice-call_source_type:0
a=x-voice-displayname:Jim
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.222:39941
a=x-voice-unique_device_id:00000004-0000-4000-8000-000000000000
a=x-voice-UF1:None
a=ivr:dialout-confirm
a=access_code:0000015
a=privs:participant
a=dialout:click_in
a=x-voice-LICENSE_URI:urn:object:r2license:user:com.example.consumer:7BF150F5-5230-4946-B3DA-66337993A01B
a=x-voice-smartproxy:USCHEQ-ASRTA01.example.net
a=user-agent:Cisco-SIPGateway/IOS-16.3.3
===========================================================================================
2020-07-02 20:28:24.968  -- SmartIOGulam() [tid:5016]
---------------------------------------------------------------------------------
NOTIFY sip:user.jc@example.com SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/tcp localhost:8443;branch=user.jc@example.com__eDial_sep__user.jc@example.com
Call-ID: callid002000000000000000000
CSeq: 42 NOTIFY
From: sip:user.jc@example.com;pool=il-ed-mara;box=il-ed-mara-06.example.net;tag=12345678
To: "jcollier" <sip:user.jc@example.com>
Contact: <sip:localhost:8443;transport=ssl>
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1593721693
X-Conf_no: 41921
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.jc@example.com
X-domain: DEFAULT
X-client-address: 192.0.2.163
X-From: user.jc@example.com
X-legs-on-server: 204
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
X-subject: User Six's Meeting Room
X-project-code: None
X-department-code: retail:USA
X-active-talker: 
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDkuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 2
X-Call-Start-Time: 1593721508.411058
Content-Length: 1253
Content-type: application/sdp

v=0
o=user.jc@example.com 1444888750 139 IN IP4 il-ed-mara-01.example.net
s=User Six's Meeting Room
i="Jim" (+15550000014) CallByPhone-URL
c=TN RFC2543 +15550000014
t=3802710347 0
m=audio 1 voice -
a=status:(203) answered  
a=mediastatus:sendrecv
a=cdr_start_time:1593721508.0
a=leg_no:1
a=remote:192.0.2.36:12058
a=flags:1050162
a=new_change
a=primary:true
a=server:il-ed-mara-06.example.net
a=clir:false:3125550011
a=far_end:192.0.2.73:41488
a=acs_guid:001367321370
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.jc@example.com
a=legCallid:646397542@192.0.2.36
a=audio_payload:PCMU
a=preferred_identity:3125550011
a=x-voice-callowner-login_alias:user.jc@example.com
a=x-voice-legowner-login_alias:user.jc@example.com
a=x-voice-call_source_type:0
a=x-voice-displayname:Jim
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.222:39941
a=x-voice-unique_device_id:00000004-0000-4000-8000-000000000000
a=x-voice-UF1:None
a=ivr:dialout-confirm
a=access_code:0000015
a=privs:participant
a=dialout:click_in
a=x-voice-LICENSE_URI:urn:object:r2license:user:com.example.consumer:7BF150F5-5230-4946-B3DA-66337993A01B
a=x-voice-smartproxy:USCHEQ-ASRTA01.example.net
a=user-agent:Cisco-SIPGateway/IOS-16.3.3
===========================================================================================
2020-07-02 20:28:25.531  -- SmartIOGulam() [tid:5016]
---------------------------------------------------------------------------------
NOTIFY sip:user.jc@example.com SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/tcp localhost:8443;branch=user.jc@example.com__eDial_sep__user.jc@example.com
Call-ID: callid002000000000000000000
CSeq: 43 NOTIFY
From: sip:user.jc@example.com;pool=il-ed-mara;box=il-ed-mara-06.example.net;tag=12345678
To: "jcollier" <sip:user.jc@example.com>
Contact: <sip:localhost:8443;transport=ssl>
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1593721693
X-Conf_no: 41921
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.jc@example.com
X-domain: DEFAULT
X-client-address: 192.0.2.163
X-From: user.jc@example.com
X-legs-on-server: 204
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
X-subject: User Six's Meeting Room
X-project-code: None
X-department-code: retail:USA
X-active-talker: 
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDkuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 2
X-Call-Start-Time: 1593721508.411058
Content-Length: 1260
Content-type: application/sdp

v=0
o=user.jc@example.com 1444888750 140 IN IP4 il-ed-mara-01.example.net
s=User Six's Meeting Room
i="Jim" (+15550000014) CallByPhone-URL
c=TN RFC2543 +15550000014
t=3802710347 0
m=audio 1 voice -
a=status:(203) answered  
a=mediastatus:sendrecv active
a=cdr_start_time:1593721508.0
a=leg_no:1
a=remote:192.0.2.36:12058
a=flags:1050162
a=new_change
a=primary:true
a=server:il-ed-mara-06.example.net
a=clir:false:3125550011
a=far_end:192.0.2.73:41488
a=acs_guid:001367321370
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.jc@example.com
a=legCallid:646397542@192.0.2.36
a=audio_payload:PCMU
a=preferred_identity:3125550011
a=x-voice-callowner-login_alias:user.jc@example.com
a=x-voice-legowner-login_alias:user.jc@example.com
a=x-voice-call_source_type:0
a=x-voice-displayname:Jim
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.222:39941
a=x-voice-unique_device_id:00000004-0000-4000-8000-000000000000
a=x-voice-UF1:None
a=ivr:dialout-confirm
a=access_code:0000015
a=privs:participant
a=dialout:click_in
a=x-voice-LICENSE_URI:urn:object:r2license:user:com.example.consumer:7BF150F5-5230-4946-B3DA-66337993A01B
a=x-voice-smartproxy:USCHEQ-ASRTA01.example.net
a=user-agent:Cisco-SIPGateway/IOS-16.3.3
===========================================================================================
2020-07-02 20:28:27.843  -- SmartIOGulam() [tid:5016]
---------------------------------------------------------------------------------
NOTIFY sip:user.jc@example.com SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/tcp localhost:8443;branch=user.jc@example.com__eDial_sep__user.jc@example.com
Call-ID: callid002000000000000000000
CSeq: 44 NOTIFY
From: sip:user.jc@example.com;pool=il-ed-mara;box=il-ed-mara-06.example.net;tag=12345678
To: "jcollier" <sip:user.jc@example.com>
Contact: <sip:localhost:8443;transport=ssl>
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1593721693
X-Conf_no: 41921
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.jc@example.com
X-domain: DEFAULT
X-client-address: 192.0.2.163
X-From: user.jc@example.com
X-legs-on-server: 205
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
X-subject: User Six's Meeting Room
X-project-code: None
X-department-code: retail:USA
X-active-talker: 
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDkuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 2
X-Call-Start-Time: 1593721508.411058
Content-Length: 1253
Content-type: application/sdp

v=0
o=user.jc@example.com 1444888750 141 IN IP4 il-ed-mara-01.example.net
s=User Six's Meeting Room
i="Jim" (+15550000014) CallByPhone-URL
c=TN RFC2543 +15550000014
t=3802710347 0
m=audio 1 voice -
a=status:(203) answered  
a=mediastatus:sendrecv
a=cdr_start_time:1593721508.0
a=leg_no:1
a=remote:192.0.2.36:12058
a=flags:1050162
a=new_change
a=primary:true
a=server:il-ed-mara-06.example.net
a=clir:false:3125550011
a=far_end:192.0.2.73:41488
a=acs_guid:001367321370
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.jc@example.com
a=legCallid:646397542@192.0.2.36
a=audio_payload:PCMU
a=preferred_identity:3125550011
a=x-voice-callowner-login_alias:user.jc@example.com
a=x-voice-legowner-login_alias:user.jc@example.com
a=x-voice-call_source_type:0
a=x-voice-displayname:Jim
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.222:39941
a=x-voice-unique_device_id:00000004-0000-4000-8000-000000000000
a=x-voice-UF1:None
a=ivr:dialout-confirm
a=access_code:0000015
a=privs:participant
a=dialout:click_in
a=x-voice-LICENSE_URI:urn:object:r2license:user:com.example.consumer:7BF150F5-5230-4946-B3DA-66337993A01B
a=x-voice-smartproxy:USCHEQ-ASRTA01.example.net
a=user-agent:Cisco-SIPGateway/IOS-16.3.3
===========================================================================================
2020-07-02 20:28:28.218  -- SmartIOGulam() [tid:5016]
---------------------------------------------------------------------------------
NOTIFY sip:user.jc@example.com SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/tcp localhost:8443;branch=user.jc@example.com__eDial_sep__user.jc@example.com
Call-ID: callid002000000000000000000
CSeq: 45 NOTIFY
From: sip:user.jc@example.com;pool=il-ed-mara;box=il-ed-mara-06.example.net;tag=12345678
To: "jcollier" <sip:user.jc@example.com>
Contact: <sip:localhost:8443;transport=ssl>
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1593721693
X-Conf_no: 41921
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.jc@example.com
X-domain: DEFAULT
X-client-address: 192.0.2.163
X-From: user.jc@example.com
X-legs-on-server: 205
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
X-subject: User Six's Meeting Room
X-project-code: None
X-department-code: retail:USA
X-active-talker: 
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDkuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 2
X-Call-Start-Time: 1593721508.411058
Content-Length: 1260
Content-type: application/sdp

v=0
o=user.jc@example.com 1444888750 142 IN IP4 il-ed-mara-01.example.net
s=User Six's Meeting Room
i="Jim" (+15550000014) CallByPhone-URL
c=TN RFC2543 +15550000014
t=3802710347 0
m=audio 1 voice -
a=status:(203) answered  
a=mediastatus:sendrecv active
a=cdr_start_time:1593721508.0
a=leg_no:1
a=remote:192.0.2.36:12058
a=flags:1050162
a=new_change
a=primary:true
a=server:il-ed-mara-06.example.net
a=clir:false:3125550011
a=far_end:192.0.2.73:41488
a=acs_guid:001367321370
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.jc@example.com
a=legCallid:646397542@192.0.2.36
a=audio_payload:PCMU
a=preferred_identity:3125550011
a=x-voice-callowner-login_alias:user.jc@example.com
a=x-voice-legowner-login_alias:user.jc@example.com
a=x-voice-call_source_type:0
a=x-voice-displayname:Jim
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.222:39941
a=x-voice-unique_device_id:00000004-0000-4000-8000-000000000000
a=x-voice-UF1:None
a=ivr:dialout-confirm
a=access_code:0000015
a=privs:participant
a=dialout:click_in
a=x-voice-LICENSE_URI:urn:object:r2license:user:com.example.consumer:7BF150F5-5230-4946-B3DA-66337993A01B
a=x-voice-smartproxy:USCHEQ-ASRTA01.example.net
a=user-agent:Cisco-SIPGateway/IOS-16.3.3
