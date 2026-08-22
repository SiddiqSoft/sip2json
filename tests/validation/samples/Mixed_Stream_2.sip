2020-07-28 14:56:31,605631
NOTIFY sip:jayne.cox@cliffordchance.com;pool=uk-ed-thames;box=ukdc1-edm14.example.net SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__jayne.cox@cliffordchance.com;received=127.0.0.1:42938:1442942896
Via: SIP/2.0/TCP 127.0.0.1:38788;received=127.0.0.1:38788:18601584
Call-ID: callid00170000000000000000000
CSeq: 1 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-thames;box=ukdc1-edm14.example.net
To: sip:jayne.cox@cliffordchance.com;pool=uk-ed-thames;box=ukdc1-edm14.example.net
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: CallStart
X-slave-site: localhost
X-last-change: 1595948191
X-Conf_no: 16732
X-rollcall: disabled
X-Call-URL: true
X-control-master: jayne.cox@cliffordchance.com
X-domain: DEFAULT
X-client-address: 192.0.2.6
X-From: jayne.cox@cliffordchance.com
X-legs-on-server: 232
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
X-leader-required: true
X-Video-UsingMCU: false
X-subject: User Seven's Meeting Room
X-project-code: None
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDguZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 1991
Content-type: application/sdp

v=0
o=jayne.cox@cliffordchance.com 909825625 104 IN IP4 uk-ed-thames-01.example.net
s=User Seven's Meeting Room
i="User%20Twenty" (+445550000007) CallByPhone-URL
c=TN RFC2543 +445550000007
t=3804936991 0
m=audio 1 voice -
a=status:(2) starting
a=mediastatus:sendrecv
a=leg_no:1
a=flags:1050144
a=new_change
a=server:ukdc1-edm14.example.net
a=clir:false:+445550000007
a=acs_guid:000952912930
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:jayne.cox@cliffordchance.com
a=legCallid:1409739130@192.0.2.10
a=cdr_start_time:1595948191.546359
a=preferred_identity:+445550000007
a=x-voice-callowner-login_alias:jayne.cox@cliffordchance.com
a=x-voice-legowner-login_alias:jayne.cox@cliffordchance.com
a=x-voice-call_source_type:0
a=x-voice-displayname:User%20Twenty
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.15:30522
a=x-voice-unique_device_id:00000002-0000-4000-8000-000000000000
a=x-voice-UF1:None
a=ivr:dialout
a=access_code:0000007
a=privs:participant
a=dialout:click_in
a=x-voice-smartproxy:usdc2-asalt04.example.net
v=0
o=jayne.cox@cliffordchance.com 909825421 104 IN IP4 uk-ed-thames-01.example.net
s=User Seven's Meeting Room
i="User%20Seven" (+445550000006) CallByPhone-URL
c=TN RFC2543 +445550000006
t=3804936991 0
m=audio 1 voice -
a=status:(1) pending
a=leg_no:2
a=flags:1054256
a=new_change
a=primary:true
a=server:ukdc1-edm14.example.net
a=clir:false:+445550000007
a=acs_guid:000952912930
a=preferred_identity:+445550000007
a=x-voice-callowner-login_alias:jayne.cox@cliffordchance.com
a=x-voice-legowner-login_alias:jayne.cox@cliffordchance.com
a=x-voice-alt-proj:None
a=x-voice-call_source_type:0
a=x-voice-displayname:User%20Seven
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.14:14794
a=x-voice-unique_device_id:00000018-0000-4000-8000-000000000000
a=x-voice-UF1:None
a=ivr:dialout
a=dialout:click_in
a=access_code:0000006
a=privs:leader
a=x-voice-smartproxy:usdc2-asalt04.example.net


2020-07-28 14:56:31,745779
NOTIFY sip:jayne.cox@cliffordchance.com;pool=uk-ed-thames;box=ukdc1-edm14.example.net SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__jayne.cox@cliffordchance.com;received=127.0.0.1:42938:1442942896
Via: SIP/2.0/TCP 127.0.0.1:38788;received=127.0.0.1:38788:18601584
Call-ID: callid00170000000000000000000
CSeq: 2 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-thames;box=ukdc1-edm14.example.net
To: sip:jayne.cox@cliffordchance.com;pool=uk-ed-thames;box=ukdc1-edm14.example.net
Contact: <sip:localhost:8443;transport=ssl>
X-slave-site: localhost
X-last-change: 1595948191
X-Conf_no: 16732
X-rollcall: disabled
X-Call-URL: true
X-control-master: jayne.cox@cliffordchance.com
X-domain: DEFAULT
X-client-address: 192.0.2.6
X-From: jayne.cox@cliffordchance.com
X-legs-on-server: 232
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
X-leader-required: true
X-Video-UsingMCU: false
X-subject: User Seven's Meeting Room
X-project-code: None
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDguZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 1100
Content-type: application/sdp

v=0
o=jayne.cox@cliffordchance.com 909825421 105 IN IP4 uk-ed-thames-01.example.net
s=User Seven's Meeting Room
i="User%20Seven" (+445550000006) CallByPhone-URL
c=TN RFC2543 +445550000006
t=3804936991 0
m=audio 1 voice -
a=status:(2) starting
a=mediastatus:sendrecv
a=leg_no:2
a=flags:1054256
a=new_change
a=primary:true
a=server:ukdc1-edm14.example.net
a=clir:false:+445550000007
a=acs_guid:000952912930
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:jayne.cox@cliffordchance.com
a=legCallid:1409739131@192.0.2.10
a=cdr_start_time:1595948191.649910
a=preferred_identity:+445550000007
a=x-voice-callowner-login_alias:jayne.cox@cliffordchance.com
a=x-voice-legowner-login_alias:jayne.cox@cliffordchance.com
a=x-voice-alt-proj:None
a=x-voice-call_source_type:0
a=x-voice-displayname:User%20Seven
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.14:14794
a=x-voice-unique_device_id:00000018-0000-4000-8000-000000000000
a=x-voice-UF1:None
a=ivr:dialout
a=dialout:click_in
a=access_code:0000006
a=privs:leader
a=x-voice-smartproxy:usdc2-asalt04.example.net


2020-07-28 14:56:40,334715
NOTIFY sip:jayne.cox@cliffordchance.com;pool=uk-ed-thames;box=ukdc1-edm14.example.net SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__jayne.cox@cliffordchance.com;received=127.0.0.1:42938:1442942896
Via: SIP/2.0/TCP 127.0.0.1:38788;received=127.0.0.1:38788:18601584
Call-ID: callid00170000000000000000000
CSeq: 5 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-thames;box=ukdc1-edm14.example.net
To: sip:jayne.cox@cliffordchance.com;pool=uk-ed-thames;box=ukdc1-edm14.example.net
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: LegAdd
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1595948200
X-Conf_no: 16732
X-rollcall: disabled
X-Call-URL: true
X-control-master: jayne.cox@cliffordchance.com
X-domain: DEFAULT
X-client-address: 192.0.2.6
X-From: jayne.cox@cliffordchance.com
X-legs-on-server: 237
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
X-leader-required: true
X-Video-UsingMCU: false
X-subject: User Seven's Meeting Room
X-project-code: None
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDguZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 1210
Content-type: application/sdp

v=0
o=jayne.cox@cliffordchance.com 909825421 116 IN IP4 uk-ed-thames-01.example.net
s=User Seven's Meeting Room
i="User%20Seven" (+445550000006) CallByPhone-URL
c=TN RFC2543 +445550000006
t=3804936991 0
m=audio 1 voice -
a=status:(203) answered
a=mediastatus:sendrecv
a=leg_no:2
a=remote:192.0.2.10:12130
a=flags:1054256
a=new_change
a=primary:true
a=server:ukdc1-edm14.example.net
a=clir:false:+445550000007
a=far_end:192.0.2.11:34730
a=acs_guid:000952912930
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:jayne.cox@cliffordchance.com
a=legCallid:1409739131@192.0.2.10
a=audio_payload:PCMA
a=cdr_start_time:1595948191.649910
a=preferred_identity:+445550000007
a=x-voice-callowner-login_alias:jayne.cox@cliffordchance.com
a=x-voice-legowner-login_alias:jayne.cox@cliffordchance.com
a=x-voice-alt-proj:None
a=x-voice-call_source_type:0
a=x-voice-displayname:User%20Seven
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.14:14794
a=x-voice-unique_device_id:00000018-0000-4000-8000-000000000000
a=x-voice-UF1:None
a=ivr:dialout
a=dialout:click_in
a=access_code:0000006
a=privs:leader
a=x-voice-smartproxy:usdc2-asalt04.example.net
a=user-agent:BN4000-3.8.1-850


2020-07-28 14:56:40,335357
NOTIFY sip:jayne.cox@cliffordchance.com;pool=uk-ed-thames;box=ukdc1-edm14.example.net SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__jayne.cox@cliffordchance.com;received=127.0.0.1:42938:1442942896
Via: SIP/2.0/TCP 127.0.0.1:38788;received=127.0.0.1:38788:18601584
Call-ID: callid00170000000000000000000
CSeq: 6 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-thames;box=ukdc1-edm14.example.net
To: sip:jayne.cox@cliffordchance.com;pool=uk-ed-thames;box=ukdc1-edm14.example.net
Contact: <sip:localhost:8443;transport=ssl>
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1595948200
X-Conf_no: 16732
X-rollcall: disabled
X-Call-URL: true
X-control-master: jayne.cox@cliffordchance.com
X-domain: DEFAULT
X-client-address: 192.0.2.6
X-From: jayne.cox@cliffordchance.com
X-legs-on-server: 237
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
X-leader-required: true
X-Video-UsingMCU: false
X-subject: User Seven's Meeting Room
X-project-code: None
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDguZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 1174
Content-type: application/sdp

v=0
o=jayne.cox@cliffordchance.com 909825625 105 IN IP4 uk-ed-thames-01.example.net
s=User Seven's Meeting Room
i="User%20Twenty" (+445550000007) CallByPhone-URL
c=TN RFC2543 +445550000007
t=3804936991 0
m=audio 1 voice -
a=status:(2) starting
a=mediastatus:sendrecv
a=leg_no:1
a=remote:192.0.2.10:12062
a=flags:1050144
a=new_change
a=server:ukdc1-edm14.example.net
a=clir:false:+445550000007
a=far_end:192.0.2.11:63906
a=acs_guid:000952912930
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:jayne.cox@cliffordchance.com
a=legCallid:1409739130@192.0.2.10
a=audio_payload:PCMA
a=cdr_start_time:1595948191.546359
a=preferred_identity:+445550000007
a=x-voice-callowner-login_alias:jayne.cox@cliffordchance.com
a=x-voice-legowner-login_alias:jayne.cox@cliffordchance.com
a=x-voice-call_source_type:0
a=x-voice-displayname:User%20Twenty
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.15:30522
a=x-voice-unique_device_id:00000002-0000-4000-8000-000000000000
a=x-voice-UF1:None
a=ivr:dialout
a=access_code:0000007
a=privs:participant
a=dialout:click_in
a=x-voice-smartproxy:usdc2-asalt04.example.net
a=user-agent:BN4000-3.8.1-850


2020-07-28 14:56:48,000360
NOTIFY sip:jayne.cox@cliffordchance.com;pool=uk-ed-thames;box=ukdc1-edm14.example.net SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__jayne.cox@cliffordchance.com;received=127.0.0.1:42938:1442942896
Via: SIP/2.0/TCP 127.0.0.1:38788;received=127.0.0.1:38788:18601584
Call-ID: callid00170000000000000000000
CSeq: 8 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-thames;box=ukdc1-edm14.example.net
To: sip:jayne.cox@cliffordchance.com;pool=uk-ed-thames;box=ukdc1-edm14.example.net
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: LegDrop
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1595948208
X-Conf_no: 16732
X-rollcall: disabled
X-Call-URL: true
X-control-master: jayne.cox@cliffordchance.com
X-domain: DEFAULT
X-client-address: 192.0.2.6
X-From: jayne.cox@cliffordchance.com
X-legs-on-server: 240
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
X-leader-required: true
X-Video-UsingMCU: false
X-subject: User Seven's Meeting Room
X-project-code: None
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDguZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 1299
Content-type: application/sdp

v=0
o=jayne.cox@cliffordchance.com 909825625 116 IN IP4 uk-ed-thames-01.example.net
s=User Seven's Meeting Room
i="User%20Twenty" (+445550000007) CallByPhone-URL
c=TN RFC2543 +445550000007
t=3804936991 3804937008
m=audio 1 voice -
a=status:(4) dropped
a=mediastatus:sendrecv
a=cause: (20001) Normal Hangup - Received SIP BYE From: A447555000012@192.0.2.11
a=leg_no:1
a=remote:192.0.2.10:12062
a=flags:1050144
a=new_change
a=server:ukdc1-edm14.example.net
a=clir:false:+445550000007
a=far_end:192.0.2.11:63906
a=acs_guid:000952912930
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:jayne.cox@cliffordchance.com
a=legCallid:1409739130@192.0.2.10
a=audio_payload:PCMA
a=cdr_start_time:1595948191.546359
a=preferred_identity:+445550000007
a=x-voice-callowner-login_alias:jayne.cox@cliffordchance.com
a=x-voice-legowner-login_alias:jayne.cox@cliffordchance.com
a=x-voice-call_source_type:0
a=x-voice-displayname:User%20Twenty
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.15:30522
a=x-voice-unique_device_id:00000002-0000-4000-8000-000000000000
a=x-voice-UF1:None
a=ivr:dialout
a=access_code:0000007
a=privs:participant
a=dialout:click_in
a=x-voice-smartproxy:usdc2-asalt04.example.net
a=user-agent:BN4000-3.8.1-850
a=cdr_stop_time:1595948208.999480


2020-07-28 14:56:49,285458
NOTIFY sip:jayne.cox@cliffordchance.com;pool=uk-ed-thames;box=ukdc1-edm14.example.net SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__jayne.cox@cliffordchance.com;received=127.0.0.1:42938:1442942896
Via: SIP/2.0/TCP 127.0.0.1:38788;received=127.0.0.1:38788:18601584
Call-ID: callid00170000000000000000000
CSeq: 1 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-thames;box=ukdc1-edm14.example.net
To: sip:jayne.cox@cliffordchance.com;pool=uk-ed-thames;box=ukdc1-edm14.example.net
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: LegDrop
X-remote-leader: true
X-slave-site: localhost
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDguZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-last-change: 1595948209
X-Conf_no: 16732
X-recording-enabled: true
X-subject: User Seven's Meeting Room
X-rollcall: disabled
X-Call-URL: true
X-control-master: jayne.cox@cliffordchance.com
X-domain: DEFAULT
X-client-address: 192.0.2.6
X-From: jayne.cox@cliffordchance.com
X-legs-on-server: 240
X-start-muted: false
X-Video-SingleView: 0
X-suppress-system-im: false
X-restrict-notify: false
X-notify-im: false
X-no-audio: false
X-no-video: false
X-Billing-code-required: false
X-Video-UsingMCU: false
X-Route-ID: 1
Content-Length: 604
Content-type: application/sdp

v=0
o=jayne.cox@cliffordchance.com 909825625 217 IN IP4 uk-ed-thames-01.example.net
s=User Seven's Meeting Room
i="User%20Twenty" (447555000012) CallByPhone-URL
c=TN RFC2543 447555000012
t=2208988801 3804937009
m=audio 1 voice -
a=status:(4) dropped
a=leg_no:1
a=flags:1180192
a=new_change
a=server:ukdc1-edm14.example.net
a=clir:false
a=x-voice-displayname:User%20Twenty
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.15:30522
a=x-voice-smartproxy:usdc2-asalt04.example.net
a=x-voice-unique_device_id:00000002-0000-4000-8000-000000000000
a=cdr_stop_time:1595948209.284676


2020-07-28 14:57:05,350850
NOTIFY sip:jayne.cox@cliffordchance.com;pool=uk-ed-thames;box=ukdc1-edm14.example.net SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__jayne.cox@cliffordchance.com;received=127.0.0.1:42938:1442942896
Via: SIP/2.0/TCP 127.0.0.1:38788;received=127.0.0.1:38788:18601584
Call-ID: callid00170000000000000000000
CSeq: 9 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-thames;box=ukdc1-edm14.example.net
To: sip:jayne.cox@cliffordchance.com;pool=uk-ed-thames;box=ukdc1-edm14.example.net
Contact: <sip:localhost:8443;transport=ssl>
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1595948225
X-Conf_no: 16732
X-rollcall: disabled
X-Call-URL: true
X-control-master: jayne.cox@cliffordchance.com
X-domain: DEFAULT
X-client-address: 192.0.2.6
X-From: jayne.cox@cliffordchance.com
X-legs-on-server: 245
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
X-leader-required: true
X-Video-UsingMCU: false
X-subject: User Seven's Meeting Room
X-project-code: None
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDguZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 1066
Content-type: application/sdp

v=0
o=jayne.cox@cliffordchance.com 909859906 104 IN IP4 uk-ed-thames-01.example.net
s=User Seven's Meeting Room
i="User%20Twenty" (+445550000007) CallByPhone-URL
c=TN RFC2543 +445550000007
t=3804937025 0
m=audio 1 voice -
a=status:(2) starting
a=mediastatus:sendrecv
a=leg_no:3
a=flags:1050144
a=new_change
a=server:ukdc1-edm14.example.net
a=clir:false:+445550000007
a=acs_guid:000952912930
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:jayne.cox@cliffordchance.com
a=legCallid:1409739147@192.0.2.10
a=cdr_start_time:1595948225.207703
a=preferred_identity:+445550000007
a=x-voice-callowner-login_alias:jayne.cox@cliffordchance.com
a=x-voice-legowner-login_alias:jayne.cox@cliffordchance.com
a=x-voice-call_source_type:0
a=x-voice-displayname:User%20Twenty
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.15:30522
a=x-voice-unique_device_id:00000002-0000-4000-8000-000000000000
a=x-voice-UF1:None
a=ivr:dialout
a=access_code:0000007
a=privs:participant
a=dialout:click_in
a=x-voice-smartproxy:usdc2-asalt04.example.net


2020-07-28 14:57:14,136189
NOTIFY sip:jayne.cox@cliffordchance.com;pool=uk-ed-thames;box=ukdc1-edm14.example.net SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__jayne.cox@cliffordchance.com;received=127.0.0.1:42938:1442942896
Via: SIP/2.0/TCP 127.0.0.1:38788;received=127.0.0.1:38788:18601584
Call-ID: callid00170000000000000000000
CSeq: 10 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-thames;box=ukdc1-edm14.example.net
To: sip:jayne.cox@cliffordchance.com;pool=uk-ed-thames;box=ukdc1-edm14.example.net
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: LegAdd
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1595948234
X-Conf_no: 16732
X-rollcall: disabled
X-Call-URL: true
X-control-master: jayne.cox@cliffordchance.com
X-domain: DEFAULT
X-client-address: 192.0.2.6
X-From: jayne.cox@cliffordchance.com
X-legs-on-server: 245
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
X-leader-required: true
X-Video-UsingMCU: false
X-subject: User Seven's Meeting Room
X-project-code: None
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDguZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 1176
Content-type: application/sdp

v=0
o=jayne.cox@cliffordchance.com 909859906 115 IN IP4 uk-ed-thames-01.example.net
s=User Seven's Meeting Room
i="User%20Twenty" (+445550000007) CallByPhone-URL
c=TN RFC2543 +445550000007
t=3804937025 0
m=audio 1 voice -
a=status:(203) answered
a=mediastatus:sendrecv
a=leg_no:3
a=remote:192.0.2.10:12282
a=flags:1050144
a=new_change
a=server:ukdc1-edm14.example.net
a=clir:false:+445550000007
a=far_end:192.0.2.11:14628
a=acs_guid:000952912930
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:jayne.cox@cliffordchance.com
a=legCallid:1409739147@192.0.2.10
a=audio_payload:PCMA
a=cdr_start_time:1595948225.207703
a=preferred_identity:+445550000007
a=x-voice-callowner-login_alias:jayne.cox@cliffordchance.com
a=x-voice-legowner-login_alias:jayne.cox@cliffordchance.com
a=x-voice-call_source_type:0
a=x-voice-displayname:User%20Twenty
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.15:30522
a=x-voice-unique_device_id:00000002-0000-4000-8000-000000000000
a=x-voice-UF1:None
a=ivr:dialout
a=access_code:0000007
a=privs:participant
a=dialout:click_in
a=x-voice-smartproxy:usdc2-asalt04.example.net
a=user-agent:BN4000-3.8.1-850


2020-07-28 15:16:03,889217
NOTIFY sip:jayne.cox@cliffordchance.com;pool=uk-ed-thames;box=ukdc1-edm14.example.net SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__jayne.cox@cliffordchance.com;received=127.0.0.1:42938:1442942896
Via: SIP/2.0/TCP 127.0.0.1:38788;received=127.0.0.1:38788:18601584
Call-ID: callid00170000000000000000000
CSeq: 12 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-thames;box=ukdc1-edm14.example.net
To: sip:jayne.cox@cliffordchance.com;pool=uk-ed-thames;box=ukdc1-edm14.example.net
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: LegDrop
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1595949363
X-Conf_no: 16732
X-rollcall: disabled
X-Call-URL: true
X-control-master: jayne.cox@cliffordchance.com
X-domain: DEFAULT
X-client-address: 192.0.2.6
X-From: jayne.cox@cliffordchance.com
X-legs-on-server: 320
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
X-leader-required: true
X-Video-UsingMCU: false
X-subject: User Seven's Meeting Room
X-project-code: None
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDguZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 1301
Content-type: application/sdp

v=0
o=jayne.cox@cliffordchance.com 909859906 137 IN IP4 uk-ed-thames-01.example.net
s=User Seven's Meeting Room
i="User%20Twenty" (+445550000007) CallByPhone-URL
c=TN RFC2543 +445550000007
t=3804937025 3804938163
m=audio 1 voice -
a=status:(4) dropped
a=mediastatus:sendrecv
a=cause: (20001) Normal Hangup - Received SIP BYE From: A447555000012@192.0.2.11
a=leg_no:3
a=remote:192.0.2.10:12282
a=flags:1050146
a=new_change
a=server:ukdc1-edm14.example.net
a=clir:false:+445550000007
a=far_end:192.0.2.11:14628
a=acs_guid:000952912930
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:jayne.cox@cliffordchance.com
a=legCallid:1409739147@192.0.2.10
a=audio_payload:PCMA
a=cdr_start_time:1595948225.207703
a=preferred_identity:+445550000007
a=x-voice-callowner-login_alias:jayne.cox@cliffordchance.com
a=x-voice-legowner-login_alias:jayne.cox@cliffordchance.com
a=x-voice-call_source_type:0
a=x-voice-displayname:User%20Twenty
a=x-voice-displayname_type:0
a=x-voice-clientaddress:192.0.2.15:30522
a=x-voice-unique_device_id:00000002-0000-4000-8000-000000000000
a=x-voice-UF1:None
a=ivr:dialout
a=access_code:0000007
a=privs:participant
a=dialout:click_in
a=x-voice-smartproxy:usdc2-asalt04.example.net
a=user-agent:BN4000-3.8.1-850
a=cdr_stop_time:1595949363.888498
