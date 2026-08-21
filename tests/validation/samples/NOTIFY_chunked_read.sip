NOTIFY sip:subscribe_to_call_events@example.com;machine SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/UDP localhost:780;branch=alexander.goncalvez@aidshealth.org__eDial_sep__subscribe_to_call_events@example.com;received=localhost
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__alexander.goncalvez@aidshealth.org;received=127.0.0.1:51780:1743178256
Via: SIP/2.0/TCP 127.0.0.1:58920;received=127.0.0.1:58920:28284832
Call-ID: callid0019000000000000000000
CSeq: 495 NOTIFY
From: sip:conference.wizard@local.host;pool=il-ed-mara;box=il-ed-mara-12.example.net
To: "alexander.goncalvez" <sip:alexander.goncalvez@aidshealth.org>
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: LegDrop
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1596215208
X-Conf_no: 168956
X-Call-URL: true
X-control-master: alexander.goncalvez@aidshealth.org
X-domain: DEFAULT
X-client-address: 10.1.206.91
X-From: alexander.goncalvez@aidshealth.org
X-legs-on-server: 367
X-start-muted: false
X-Video-SingleView: 0
X-suppress-system-im: false
X-restrict-notify: false
X-notify-im: false
X-no-audio: false
X-no-video: false
X-dialout: allowed
X-Billing-code-required: false
X-rollcall: enabled
X-recording-enabled: true
X-Video-UsingMCU: false
X-subject: PHD Testing
X-project-code: 161000
X-active-talker: 
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDIuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 2
Content-Length: 2700
Content-type: application/sdp

v=0
o=alexander.goncalvez@aidshealth.org 1177718421 145 IN IP4 il-ed-mara-01.example.net
s=PHD Testing
i="User%20Eighteen" (+15550000017) CallByPhone-URL
c=TN RFC2543 +15550000017
t=3805200650 3805204008
m=audio 1 voice -
a=status:(4) dropped
a=mediastatus:sendrecv 
a=cause: (20001) Normal Hangup - Received SIP BYE From: 16783659173@192.0.2.73
a=leg_no:21
a=remote:192.0.2.73:12094
a=flags:1050144
a=new_change
a=server:il-ed-mara-12.example.net
a=clir:false:18755500010
a=recording:uri:http://il-ed-mara-12.example.net/slides/rollcalls/rcHe0HCH
a=far_end:192.0.2.73:10656
a=acs_guid:001123671132
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:alexander.goncalvez@aidshealth.org
a=legCallid:1001668592@192.0.2.73
a=audio_payload:PCMU
a=cdr_start_time:1596211850.816548
a=preferred_identity:18755500010
a=x-ring2-callowner-login_alias:alexander.goncalvez@aidshealth.org
a=x-ring2-legowner-login_alias:alexander.goncalvez@aidshealth.org
a=x-ring2-call_source_type:0
a=x-ring2-displayname:User%20Eighteen
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:192.0.2.12:14277
a=x-ring2-unique_device_id:00000012-0000-4000-8000-000000000000
a=x-ring2-UF1:161000
a=ivr:dialout-confirm
a=access_code:0000017
a=privs:participant
a=dialout:click_in
a=x-ring2-smartproxy:usaze-asalt01.example.net
a=user-agent:Cisco-SIPGateway/IOS-16.3.3
a=cdr_stop_time:1596215208.479473
v=0
o=alexander.goncalvez@aidshealth.org 1177749437 129 IN IP4 il-ed-mara-01.example.net
s=PHD Testing
i="User%20Thirteen" (+15550000016) CallByPhone-URL
c=TN RFC2543 +15550000016
t=3805200681 0
m=audio 1 voice -
a=status:(203) answered  
a=mediastatus:sendrecv active
a=leg_no:22
a=remote:192.0.2.73:12036
a=flags:1050146
a=new_change
a=server:il-ed-mara-12.example.net
a=clir:false:18755500010
a=recording:uri:http://il-ed-mara-12.example.net/slides/rollcalls/rcbzVr60
a=far_end:192.0.2.73:43050
a=acs_guid:001123671132
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:alexander.goncalvez@aidshealth.org
a=legCallid:1001668593@192.0.2.73
a=audio_payload:PCMU
a=cdr_start_time:1596211881.878508
a=preferred_identity:18755500010
a=x-ring2-callowner-login_alias:alexander.goncalvez@aidshealth.org
a=x-ring2-legowner-login_alias:alexander.goncalvez@aidshealth.org
a=x-ring2-call_source_type:0
a=x-ring2-displayname:User%20Thirteen
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:192.0.2.2:14598
a=x-ring2-unique_device_id:00000005-0000-4000-8000-000000000000
a=x-ring2-UF1:161000
a=ivr:dialout-confirm
a=access_code:0000017
a=privs:participant
a=dialout:click_in
a=x-ring2-smartproxy:usaze-asalt01.example.net
a=user-agent:Cisco-SIPGateway/IOS-16.3.3
NOTIFY sip:subscribe_to_call_events@example.com;machine SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/UDP localhost:780;branch=hbcdhvbdfh67@example.org__eDial_sep__subscribe_to_call_events@example.com;received=localhost
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__hbcdhvbdfh67@example.org;received=127.0.0.1:51204:1107030160
Via: SIP/2.0/TCP 127.0.0.1:33942;received=127.0.0.1:33942:12280832
Call-ID: callid00220000000000000000
CSeq: 1 NOTIFY
From: sip:conference.wizard@local.host;pool=il-ed-aras;box=il-ed-aras-05.example-corp.com
To: "hbcdhvbdfh67" <sip:hbcdhvbdfh67@example.org>
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: CallStart
X-slave-site: localhost
X-last-change: 1596099474
X-drop-with-leader: false
X-Conf_no: 922
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: hbcdhvbdfh67@example.org
X-domain: DEFAULT
X-client-address: 127.0.0.1
X-From: hbcdhvbdfh67@example.org
X-legs-on-server: 1
X-Sticky: 1
X-rss-id: 
X-start-muted: false
X-Video-SingleView: -1
X-suppress-system-im: false
X-restrict-notify: false
X-notify-im: false
X-no-audio: false
X-no-video: false
X-dialout: allowed
X-Billing-code-required: true
X-recording-enabled: true
X-Video-UsingMCU: false
X-subject: hdsbfsjh jcbdhv's Meeting Room
X-project-code: None
X-department-code: retail
X-reserved-ports: 0
X-restrict-participants: false
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMTIuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 2
Content-Length: 792
Content-type: application/sdp

v=0
o=hbcdhvbdfh67@example.org 95023660363472 846985136 IN IP4 il-ed-aras-01.example-corp.com
s=
i=+15550000020 (+15550000020) CallByPhone-URL
c=TN RFC2543 +15550000020
t=3805088241 0
m=audio 1 voice -
a=status:(2) starting
a=mediastatus:sendrecv
a=leg_no:1
a=remote:198.51.100.73:12000
a=flags:1053728
a=new_change
a=server:il-ed-aras-05.example-corp.com
a=clir:false
a=far_end:192.0.2.7:35186
a=acs_guid:000923892239
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:14155500008@198.51.100.73
a=audio_payload:PCMU
a=local_mos:42
a=cdr_start_time:1596099441.799922
a=access_code:0000011
a=privs:leader
a=ivr:dialin
a=creator:hbcdhvbdfh67@example.org
a=dialin:0000011:6660016205@192.0.2.7-$$-1
a=dest_phone:14155500008
a=user-agent:Cisco-SIPGateway/IOS-15.5.3.S5
