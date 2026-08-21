NOTIFY sip:user.jc@example.com SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/tcp localhost:8443;branch=user.jc@example.com__eDial_sep__user.jc@example.com
Call-ID: callid002000000000000000000
CSeq: 31 NOTIFY
From: sip:user.jc@example.com;pool=il-ed-mara;box=il-ed-mara-06.example.net;tag=12345678
To: "jcollier" <sip:user.jc@example.com>
Contact: <sip:localhost:8443;transport=ssl>
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1593721650
X-Conf_no: 41921
X-rollcall: disabled
X-Call-URL: true
Warning: X-200 From SUBSCRIBE
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
a=x-ring2-callowner-login_alias:user.jc@example.com
a=x-ring2-legowner-login_alias:user.jc@example.com
a=x-ring2-call_source_type:0
a=x-ring2-displayname:Jim
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:192.0.2.222:39941
a=x-ring2-unique_device_id:00000004-0000-4000-8000-000000000000
a=x-ring2-UF1:None
a=ivr:dialout-confirm
a=access_code:0000015
a=privs:participant
a=dialout:click_in
a=x-ring2-LICENSE_URI:urn:object:r2license:user:com.example.consumer:7BF150F5-5230-4946-B3DA-66337993A01B
a=x-ring2-smartproxy:USCHEQ-ASRTA01.example.net
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
a=x-ring2-callowner-login_alias:user.jc@example.com
a=x-ring2-legowner-login_alias:user.jc@example.com
a=x-ring2-call_source_type:0
a=x-ring2-displayname:User%20Ten
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:192.0.2.222:681
a=x-ring2-unique_device_id:00000009-0000-4000-8000-000000000000
a=x-ring2-UF1:None
a=ivr:dialout-confirm
a=access_code:0000015
a=privs:participant
a=dialout:click_in
a=x-ring2-LICENSE_URI:urn:object:r2license:user:com.example.consumer:7BF150F5-5230-4946-B3DA-66337993A01B
a=x-ring2-smartproxy:USCHEQ-ASRTA01.example.net
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
a=x-ring2-callowner-login_alias:user.jc@example.com
a=x-ring2-legowner-login_alias:user.jc@example.com
a=x-ring2-call_source_type:0
a=x-ring2-displayname:Tanya
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:192.0.2.222:51156
a=x-ring2-unique_device_id:00000003-0000-4000-8000-000000000000
a=x-ring2-UF1:None
a=ivr:dialout-confirm
a=access_code:0000015
a=privs:participant
a=dialout:click_in
a=x-ring2-LICENSE_URI:urn:object:r2license:user:com.example.consumer:7BF150F5-5230-4946-B3DA-66337993A01B
a=x-ring2-smartproxy:USCHEQ-ASRTA01.example.net
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
a=x-ring2-callowner-login_alias:user.jc@example.com
a=x-ring2-legowner-login_alias:user.jc@example.com
a=x-ring2-call_source_type:0
a=x-ring2-displayname:User%20Fifteen
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:192.0.2.222:49786
a=x-ring2-unique_device_id:0000000a-0000-4000-8000-000000000000
a=x-ring2-UF1:None
a=ivr:dialout-confirm
a=access_code:0000015
a=privs:participant
a=dialout:click_in
a=x-ring2-LICENSE_URI:urn:object:r2license:user:com.example.consumer:7BF150F5-5230-4946-B3DA-66337993A01B
a=x-ring2-smartproxy:USCHEQ-ASRTA01.example.net
a=user-agent:Cisco-SIPGateway/IOS-16.3.3
