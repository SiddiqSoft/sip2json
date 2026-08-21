2020-07-28 04:56:46,974547
NOTIFY sip:user.mz@example.com;pool=uk-ed-thames;box=ukdc1-edm13.example.net SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__user.mz@example.com;received=127.0.0.1:34862:1442942896
Via: SIP/2.0/TCP 127.0.0.1:48868;received=127.0.0.1:48868:26772976
Call-ID: call-id-0001.00000000000000server01.example.net
CSeq: 1 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-thames;box=ukdc1-edm13.example.net
To: sip:user.mz@example.com;pool=uk-ed-thames;box=ukdc1-edm13.example.net
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: CallStart
X-slave-site: localhost
X-last-change: 1595912206
X-drop-with-leader: false
X-Conf_no: 9717
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.mz@example.com
X-domain: DEFAULT
X-client-address: 127.0.0.1
X-From: user.mz@example.com
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
X-Billing-code-required: false
X-recording-enabled: true
X-leader-required: true
X-Video-UsingMCU: false
X-subject: User Four's Meeting Room
X-project-code: None
X-reserved-ports: 0
X-restrict-participants: false
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDEuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 773
Content-type: application/sdp

v=0
o=user.mz@example.com 57046985175711 847315497 IN IP4 uk-ed-thames-01.example.net
s=
i=+44555000000001 (+44555000000001) CallByPhone-URL
c=TN RFC2543 +44555000000001
t=3804900991 0
m=audio 1 voice -
a=status:(2) starting
a=mediastatus:sendrecv
a=leg_no:1
a=remote:203.0.113.41:12002
a=flags:1049632
a=new_change
a=server:ukdc1-edm13.example.net
a=clir:false
a=far_end:10.44.177.35:38880
a=acs_guid:000471698977
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:+44044987799@203.0.113.41
a=audio_payload:PCMU
a=cdr_start_time:1595912191.862391
a=access_code:00000002
a=privs:participant
a=ivr:dialin
a=creator:user.mz@example.com
a=dialin:00000002:6660013449@10.44.177.35-$$-1
a=dest_phone:+44044987799
a=user-agent:Cisco-SIPGateway/IOS-16.9.4


2020-07-28 04:56:48,126850
NOTIFY sip:user.mz@example.com;pool=uk-ed-thames;box=ukdc1-edm13.example.net SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__user.mz@example.com;received=127.0.0.1:34862:1442942896
Via: SIP/2.0/TCP 127.0.0.1:48868;received=127.0.0.1:48868:26772976
Call-ID: call-id-0001.00000000000000server01.example.net
CSeq: 3 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-thames;box=ukdc1-edm13.example.net
To: sip:user.mz@example.com;pool=uk-ed-thames;box=ukdc1-edm13.example.net
Contact: <sip:localhost:8443;transport=ssl>
X-slave-site: localhost
X-last-change: 1595912208
X-drop-with-leader: false
X-Conf_no: 9717
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.mz@example.com
X-domain: DEFAULT
X-client-address: 127.0.0.1
X-From: user.mz@example.com
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
X-Billing-code-required: false
X-recording-enabled: true
X-leader-required: true
X-Video-UsingMCU: false
X-subject: User Four's Meeting Room
X-project-code: None
X-reserved-ports: 0
X-restrict-participants: false
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDEuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 775
Content-type: application/sdp

v=0
o=user.mz@example.com 57046985175711 847315508 IN IP4 uk-ed-thames-01.example.net
s=
i=+44555000000001 (+44555000000001) CallByPhone-URL
c=TN RFC2543 +44555000000001
t=3804900991 0
m=audio 1 voice -
a=status:(203) answered
a=mediastatus:sendrecv
a=leg_no:1
a=remote:203.0.113.41:12002
a=flags:1049632
a=new_change
a=server:ukdc1-edm13.example.net
a=clir:false
a=far_end:10.44.177.35:38880
a=acs_guid:000471698977
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:+44044987799@203.0.113.41
a=audio_payload:PCMU
a=cdr_start_time:1595912191.862391
a=access_code:00000002
a=privs:participant
a=ivr:dialin
a=creator:user.mz@example.com
a=dialin:00000002:6660013449@10.44.177.35-$$-1
a=dest_phone:+44044987799
a=user-agent:Cisco-SIPGateway/IOS-16.9.4


2020-07-28 04:56:48,128405
NOTIFY sip:user.mz@example.com;pool=uk-ed-thames;box=ukdc1-edm13.example.net SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__user.mz@example.com;received=127.0.0.1:34862:1442942896
Via: SIP/2.0/TCP 127.0.0.1:48868;received=127.0.0.1:48868:26772976
Call-ID: call-id-0001.00000000000000server01.example.net
CSeq: 5 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-thames;box=ukdc1-edm13.example.net
To: sip:user.mz@example.com;pool=uk-ed-thames;box=ukdc1-edm13.example.net
Contact: <sip:localhost:8443;transport=ssl>
X-slave-site: localhost
X-last-change: 1595912208
X-drop-with-leader: false
X-Conf_no: 9717
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.mz@example.com
X-domain: DEFAULT
X-client-address: 127.0.0.1
X-From: user.mz@example.com
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
X-Billing-code-required: false
X-recording-enabled: true
X-leader-required: true
X-Video-UsingMCU: false
X-subject: User Four's Meeting Room
X-project-code: None
X-reserved-ports: 0
X-restrict-participants: false
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDEuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 779
Content-type: application/sdp

v=0
o=user.mz@example.com 57046985175711 847315520 IN IP4 uk-ed-thames-01.example.net
s=
i=+44555000000001 (+44555000000001) CallByPhone-URL
c=TN RFC2543 +44555000000001
t=3804900991 0
m=audio 1 voice -
a=status:(205) answered hold
a=mediastatus:nomedia
a=leg_no:1
a=remote:203.0.113.41:12002
a=flags:1049634
a=new_change
a=server:ukdc1-edm13.example.net
a=clir:false
a=far_end:10.44.177.35:38880
a=acs_guid:000471698977
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:+44044987799@203.0.113.41
a=audio_payload:PCMU
a=cdr_start_time:1595912191.862391
a=access_code:00000002
a=privs:participant
a=ivr:dialin
a=creator:user.mz@example.com
a=dialin:00000002:6660013449@10.44.177.35-$$-1
a=dest_phone:+44044987799
a=user-agent:Cisco-SIPGateway/IOS-16.9.4


2020-07-28 04:56:55,973631
NOTIFY sip:user.mz@example.com;pool=uk-ed-thames;box=ukdc1-edm13.example.net SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__user.mz@example.com;received=127.0.0.1:34862:1442942896
Via: SIP/2.0/TCP 127.0.0.1:48868;received=127.0.0.1:48868:26772976
Call-ID: call-id-0001.00000000000000server01.example.net
CSeq: 6 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-thames;box=ukdc1-edm13.example.net
To: sip:user.mz@example.com;pool=uk-ed-thames;box=ukdc1-edm13.example.net
Contact: <sip:localhost:8443;transport=ssl>
X-slave-site: localhost
X-last-change: 1595912215
X-drop-with-leader: false
X-Conf_no: 9717
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.mz@example.com
X-domain: DEFAULT
X-client-address: 127.0.0.1
X-From: user.mz@example.com
X-legs-on-server: 2
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
X-Billing-code-required: false
X-recording-enabled: true
X-leader-required: true
X-Video-UsingMCU: false
X-subject: User Four's Meeting Room
X-project-code: None
X-reserved-ports: 0
X-restrict-participants: false
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDEuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 773
Content-type: application/sdp

v=0
o=user.mz@example.com 57176408823365 847315503 IN IP4 uk-ed-thames-01.example.net
s=
i=+44555000000002 (+44555000000002) CallByPhone-URL
c=TN RFC2543 +44555000000002
t=3804900978 0
m=audio 1 voice -
a=status:(2) starting
a=mediastatus:sendrecv
a=leg_no:2
a=remote:203.0.113.41:12000
a=flags:1049632
a=new_change
a=server:ukdc1-edm13.example.net
a=clir:false
a=far_end:10.44.177.35:42752
a=acs_guid:000471698977
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:+44044987799@203.0.113.41
a=audio_payload:PCMU
a=cdr_start_time:1595912178.258632
a=access_code:00000002
a=privs:participant
a=ivr:dialin
a=creator:user.mz@example.com
a=dialin:00000002:6660013448@10.44.177.35-$$-1
a=dest_phone:+44044987799
a=user-agent:Cisco-SIPGateway/IOS-16.9.4


2020-07-28 04:56:55,974758
NOTIFY sip:user.mz@example.com;pool=uk-ed-thames;box=ukdc1-edm13.example.net SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__user.mz@example.com;received=127.0.0.1:34862:1442942896
Via: SIP/2.0/TCP 127.0.0.1:48868;received=127.0.0.1:48868:26772976
Call-ID: call-id-0001.00000000000000server01.example.net
CSeq: 7 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-thames;box=ukdc1-edm13.example.net
To: sip:user.mz@example.com;pool=uk-ed-thames;box=ukdc1-edm13.example.net
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: LegAdd
X-slave-site: localhost
X-last-change: 1595912215
X-drop-with-leader: false
X-Conf_no: 9717
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.mz@example.com
X-domain: DEFAULT
X-client-address: 127.0.0.1
X-From: user.mz@example.com
X-legs-on-server: 2
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
X-Billing-code-required: false
X-recording-enabled: true
X-leader-required: true
X-Video-UsingMCU: false
X-subject: User Four's Meeting Room
X-project-code: None
X-reserved-ports: 0
X-restrict-participants: false
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDEuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 791
Content-type: application/sdp

v=0
o=user.mz@example.com 57176408823365 847315514 IN IP4 uk-ed-thames-01.example.net
s=
i=+44555000000002 (+44555000000002) CallByPhone-URL
c=TN RFC2543 +44555000000002
t=3804900978 0
m=audio 1 voice -
a=status:(203) answered
a=mediastatus:sendrecv
a=leg_no:2
a=remote:203.0.113.41:12000
a=flags:1049632
a=new_change
a=server:ukdc1-edm13.example.net
a=clir:false
a=far_end:10.44.177.35:42752
a=conference:1
a=acs_guid:000471698977
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:+44044987799@203.0.113.41
a=audio_payload:PCMU
a=cdr_start_time:1595912178.258632
a=access_code:00000002
a=privs:participant
a=ivr:dialin
a=creator:user.mz@example.com
a=dialin:00000002:6660013448@10.44.177.35-$$-1
a=dest_phone:+44044987799
a=user-agent:Cisco-SIPGateway/IOS-16.9.4


2020-07-28 04:56:57,218523
NOTIFY sip:user.mz@example.com;pool=uk-ed-thames;box=ukdc1-edm13.example.net SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__user.mz@example.com;received=127.0.0.1:34862:1442942896
Via: SIP/2.0/TCP 127.0.0.1:48868;received=127.0.0.1:48868:26772976
Call-ID: call-id-0001.00000000000000server01.example.net
CSeq: 10 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-thames;box=ukdc1-edm13.example.net
To: sip:user.mz@example.com;pool=uk-ed-thames;box=ukdc1-edm13.example.net
Contact: <sip:localhost:8443;transport=ssl>
X-slave-site: localhost
X-last-change: 1595912217
X-drop-with-leader: false
X-Conf_no: 9717
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.mz@example.com
X-domain: DEFAULT
X-client-address: 127.0.0.1
X-From: user.mz@example.com
X-legs-on-server: 2
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
X-Billing-code-required: false
X-recording-enabled: true
X-leader-required: true
X-Video-UsingMCU: false
X-subject: User Four's Meeting Room
X-project-code: None
X-reserved-ports: 0
X-restrict-participants: false
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDEuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 779
Content-type: application/sdp

v=0
o=user.mz@example.com 57176408823365 847315537 IN IP4 uk-ed-thames-01.example.net
s=
i=+44555000000002 (+44555000000002) CallByPhone-URL
c=TN RFC2543 +44555000000002
t=3804900978 0
m=audio 1 voice -
a=status:(205) answered hold
a=mediastatus:nomedia
a=leg_no:2
a=remote:203.0.113.41:12000
a=flags:1049634
a=new_change
a=server:ukdc1-edm13.example.net
a=clir:false
a=far_end:10.44.177.35:42752
a=acs_guid:000471698977
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:+44044987799@203.0.113.41
a=audio_payload:PCMU
a=cdr_start_time:1595912178.258632
a=access_code:00000002
a=privs:participant
a=ivr:dialin
a=creator:user.mz@example.com
a=dialin:00000002:6660013448@10.44.177.35-$$-1
a=dest_phone:+44044987799
a=user-agent:Cisco-SIPGateway/IOS-16.9.4


2020-07-28 04:59:14,635475
NOTIFY sip:user.mz@example.com;pool=hk-ed-pearl;box=hk-ed-pearl-01.example.net SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__user.mz@example.com;received=127.0.0.1:45930:1442942896
Via: SIP/2.0/TCP 127.0.0.1:38454;received=127.0.0.1:38454:20705328
Call-ID: call-id-0001.00000000000000server01.example.net
CSeq: 1 NOTIFY
From: sip:conference.wizard@local.host;pool=hk-ed-pearl;box=hk-ed-pearl-01.example.net
To: sip:user.mz@example.com;pool=hk-ed-pearl;box=hk-ed-pearl-01.example.net
Contact: <sip:localhost:8443;transport=ssl>
X-max-leg-no: 5
X-drop-with-leader: false
X-rss-id:
X-Sticky: 1
X-restrict-participants: false
X-no-unmute: false
X-slave-site: localhost
X-last-change: 1595912354
X-Conf_no: 9717
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.mz@example.com
X-domain: DEFAULT
X-client-address: 192.0.2.6
X-From: user.mz@example.com
X-legs-on-server: 78
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
X-subject: User Four's Meeting Room
X-project-code: None
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDEuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 1041
Content-type: application/sdp

v=0
o=user.mz@example.com 873988796 103 IN IP4 uk-ed-thames-01.example.net
s=User Four's Meeting Room
i="User%20Seventeen" (+61555000002) CallByPhone-URL
c=TN RFC2543 +61555000002
t=3804901154 0
m=audio 1 voice -
a=status:(2) starting
a=mediastatus:sendrecv
a=leg_no:3
a=flags:1181216
a=new_change
a=server:hk-ed-pearl-01.example.net
a=clir:false:61255500013
a=acs_guid:000446823944
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.mz@example.com
a=legCallid:11207077@198.51.100.71
a=cdr_start_time:1595912354.404381
a=preferred_identity:61255500013
a=x-ring2-callowner-login_alias:user.mz@example.com
a=x-ring2-legowner-login_alias:user.mz@example.com
a=x-ring2-call_source_type:0
a=x-ring2-displayname:User%20Seventeen
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:192.0.2.3:60852
a=x-ring2-unique_device_id:00000014-0000-4000-8000-000000000000
a=x-ring2-UF1:None
a=ivr:dialout-confirm
a=access_code:00000002
a=privs:participant
a=dialout:click_in
a=x-ring2-smartproxy:usdc2-asalt04.example.net


2020-07-28 04:59:27,273665
NOTIFY sip:user.mz@example.com;pool=hk-ed-pearl;box=hk-ed-pearl-01.example.net SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__user.mz@example.com;received=127.0.0.1:45930:1442942896
Via: SIP/2.0/TCP 127.0.0.1:38454;received=127.0.0.1:38454:20705328
Call-ID: call-id-0001.00000000000000server01.example.net
CSeq: 5 NOTIFY
From: sip:conference.wizard@local.host;pool=hk-ed-pearl;box=hk-ed-pearl-01.example.net
To: sip:user.mz@example.com;pool=hk-ed-pearl;box=hk-ed-pearl-01.example.net
Contact: <sip:localhost:8443;transport=ssl>
X-max-leg-no: 5
X-CallEvent: LegAdd
X-drop-with-leader: false
X-rss-id:
X-Sticky: 1
X-slave-site: localhost
X-last-change: 1595912367
X-Conf_no: 9717
X-restrict-participants: false
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.mz@example.com
X-domain: DEFAULT
X-client-address: 192.0.2.6
X-From: user.mz@example.com
X-legs-on-server: 84
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
X-subject: User Four's Meeting Room
X-project-code: None
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDEuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 1171
Content-type: application/sdp

v=0
o=user.mz@example.com 873988796 114 IN IP4 uk-ed-thames-01.example.net
s=User Four's Meeting Room
i="User%20Seventeen" (+61555000002) CallByPhone-URL
c=TN RFC2543 +61555000002
t=3804901154 0
m=audio 1 voice -
a=status:(203) answered
a=mediastatus:sendrecv
a=leg_no:3
a=remote:198.51.100.71:12120
a=flags:1181216
a=new_change
a=server:hk-ed-pearl-01.example.net
a=clir:false:61255500013
a=far_end:198.51.100.66:13198
a=acs_guid:000446823944
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.mz@example.com
a=legCallid:11207077@198.51.100.71
a=audio_payload:PCMU
a=cdr_start_time:1595912354.404381
a=preferred_identity:61255500013
a=x-ring2-callowner-login_alias:user.mz@example.com
a=x-ring2-legowner-login_alias:user.mz@example.com
a=x-ring2-call_source_type:0
a=x-ring2-displayname:User%20Seventeen
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:192.0.2.3:60852
a=x-ring2-unique_device_id:00000014-0000-4000-8000-000000000000
a=x-ring2-UF1:None
a=ivr:dialout-confirm
a=access_code:00000002
a=privs:participant
a=dialout:click_in
a=x-ring2-smartproxy:usdc2-asalt04.example.net
a=user-agent:Cisco-SIPGateway/IOS-15.5.2.S4


2020-07-28 04:59:27,430917
NOTIFY sip:user.mz@example.com;pool=hk-ed-pearl;box=hk-ed-pearl-01.example.net SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__user.mz@example.com;received=127.0.0.1:45930:1442942896
Via: SIP/2.0/TCP 127.0.0.1:38454;received=127.0.0.1:38454:20705328
Call-ID: call-id-0001.00000000000000server01.example.net
CSeq: 7 NOTIFY
From: sip:conference.wizard@local.host;pool=hk-ed-pearl;box=hk-ed-pearl-01.example.net
To: sip:user.mz@example.com;pool=hk-ed-pearl;box=hk-ed-pearl-01.example.net
Contact: <sip:localhost:8443;transport=ssl>
X-max-leg-no: 5
X-drop-with-leader: false
X-rss-id:
X-Sticky: 1
X-slave-site: localhost
X-last-change: 1595912367
X-Conf_no: 9717
X-restrict-participants: false
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.mz@example.com
X-domain: DEFAULT
X-client-address: 192.0.2.6
X-From: user.mz@example.com
X-legs-on-server: 84
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
X-subject: User Four's Meeting Room
X-project-code: None
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDEuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 1175
Content-type: application/sdp

v=0
o=user.mz@example.com 873988796 126 IN IP4 uk-ed-thames-01.example.net
s=User Four's Meeting Room
i="User%20Seventeen" (+61555000002) CallByPhone-URL
c=TN RFC2543 +61555000002
t=3804901154 0
m=audio 1 voice -
a=status:(205) answered hold
a=mediastatus:nomedia
a=leg_no:3
a=remote:198.51.100.71:12120
a=flags:1181218
a=new_change
a=server:hk-ed-pearl-01.example.net
a=clir:false:61255500013
a=far_end:198.51.100.66:13198
a=acs_guid:000446823944
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.mz@example.com
a=legCallid:11207077@198.51.100.71
a=audio_payload:PCMU
a=cdr_start_time:1595912354.404381
a=preferred_identity:61255500013
a=x-ring2-callowner-login_alias:user.mz@example.com
a=x-ring2-legowner-login_alias:user.mz@example.com
a=x-ring2-call_source_type:0
a=x-ring2-displayname:User%20Seventeen
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:192.0.2.3:60852
a=x-ring2-unique_device_id:00000014-0000-4000-8000-000000000000
a=x-ring2-UF1:None
a=ivr:dialout-confirm
a=access_code:00000002
a=privs:participant
a=dialout:click_in
a=x-ring2-smartproxy:usdc2-asalt04.example.net
a=user-agent:Cisco-SIPGateway/IOS-15.5.2.S4


2020-07-28 05:00:25,127350
NOTIFY sip:user.mz@example.com;pool=hk-ed-pearl;box=hk-ed-pearl-01.example.net SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__user.mz@example.com;received=127.0.0.1:45930:1442942896
Via: SIP/2.0/TCP 127.0.0.1:38454;received=127.0.0.1:38454:20705328
Call-ID: call-id-0001.00000000000000server01.example.net
CSeq: 8 NOTIFY
From: sip:conference.wizard@local.host;pool=hk-ed-pearl;box=hk-ed-pearl-01.example.net
To: sip:user.mz@example.com;pool=hk-ed-pearl;box=hk-ed-pearl-01.example.net
Contact: <sip:localhost:8443;transport=ssl>
X-max-leg-no: 6
X-drop-with-leader: false
X-rss-id:
X-Sticky: 1
X-slave-site: localhost
X-last-change: 1595912425
X-Conf_no: 9717
X-restrict-participants: false
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.mz@example.com
X-domain: DEFAULT
X-client-address: 192.0.2.6
X-From: user.mz@example.com
X-legs-on-server: 102
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
X-subject: User Four's Meeting Room
X-project-code: None
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDEuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 1054
Content-type: application/sdp

v=0
o=user.mz@example.com 874059406 103 IN IP4 uk-ed-thames-01.example.net
s=User Four's Meeting Room
i="User%20Four" (+61555000001) CallByPhone-URL
c=TN RFC2543 +61555000001
t=3804901224 0
m=audio 1 voice -
a=status:(2) starting
a=mediastatus:sendrecv
a=leg_no:6
a=flags:1185312
a=new_change
a=server:hk-ed-pearl-01.example.net
a=clir:false:61255500013
a=acs_guid:000446823944
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.mz@example.com
a=legCallid:11207097@198.51.100.71
a=cdr_start_time:1595912424.968570
a=preferred_identity:61255500013
a=x-ring2-callowner-login_alias:user.mz@example.com
a=x-ring2-legowner-login_alias:user.mz@example.com
a=x-ring2-alt-proj:None
a=x-ring2-call_source_type:0
a=x-ring2-displayname:User%20Four
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:203.0.113.56:54290
a=x-ring2-unique_device_id:00000006-0000-4000-8000-000000000000
a=x-ring2-UF1:None
a=ivr:dialout-confirm
a=dialout:click_in
a=access_code:00000001
a=privs:leader
a=x-ring2-smartproxy:usdc2-asalt04.example.net


2020-07-28 05:00:38,333252
NOTIFY sip:user.mz@example.com;pool=hk-ed-pearl;box=hk-ed-pearl-01.example.net SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__user.mz@example.com;received=127.0.0.1:45930:1442942896
Via: SIP/2.0/TCP 127.0.0.1:38454;received=127.0.0.1:38454:20705328
Call-ID: call-id-0001.00000000000000server01.example.net
CSeq: 9 NOTIFY
From: sip:conference.wizard@local.host;pool=hk-ed-pearl;box=hk-ed-pearl-01.example.net
To: sip:user.mz@example.com;pool=hk-ed-pearl;box=hk-ed-pearl-01.example.net
Contact: <sip:localhost:8443;transport=ssl>
X-max-leg-no: 6
X-CallEvent: LegAdd
X-drop-with-leader: false
X-rss-id:
X-Sticky: 1
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1595912438
X-Conf_no: 9717
X-restrict-participants: false
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.mz@example.com
X-domain: DEFAULT
X-client-address: 192.0.2.6
X-From: user.mz@example.com
X-legs-on-server: 106
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
X-subject: User Four's Meeting Room
X-project-code: None
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDEuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 1184
Content-type: application/sdp

v=0
o=user.mz@example.com 874059406 114 IN IP4 uk-ed-thames-01.example.net
s=User Four's Meeting Room
i="User%20Four" (+61555000001) CallByPhone-URL
c=TN RFC2543 +61555000001
t=3804901224 0
m=audio 1 voice -
a=status:(203) answered
a=mediastatus:sendrecv
a=leg_no:6
a=remote:198.51.100.71:12150
a=flags:1185312
a=new_change
a=server:hk-ed-pearl-01.example.net
a=clir:false:61255500013
a=far_end:198.51.100.66:12246
a=acs_guid:000446823944
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.mz@example.com
a=legCallid:11207097@198.51.100.71
a=audio_payload:PCMU
a=cdr_start_time:1595912424.968570
a=preferred_identity:61255500013
a=x-ring2-callowner-login_alias:user.mz@example.com
a=x-ring2-legowner-login_alias:user.mz@example.com
a=x-ring2-alt-proj:None
a=x-ring2-call_source_type:0
a=x-ring2-displayname:User%20Four
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:203.0.113.56:54290
a=x-ring2-unique_device_id:00000006-0000-4000-8000-000000000000
a=x-ring2-UF1:None
a=ivr:dialout-confirm
a=dialout:click_in
a=access_code:00000001
a=privs:leader
a=x-ring2-smartproxy:usdc2-asalt04.example.net
a=user-agent:Cisco-SIPGateway/IOS-15.5.2.S4


2020-07-28 05:00:38,334165
NOTIFY sip:user.mz@example.com;pool=hk-ed-pearl;box=hk-ed-pearl-01.example.net SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__user.mz@example.com;received=127.0.0.1:45930:1442942896
Via: SIP/2.0/TCP 127.0.0.1:38454;received=127.0.0.1:38454:20705328
Call-ID: call-id-0001.00000000000000server01.example.net
CSeq: 10 NOTIFY
From: sip:conference.wizard@local.host;pool=hk-ed-pearl;box=hk-ed-pearl-01.example.net
To: sip:user.mz@example.com;pool=hk-ed-pearl;box=hk-ed-pearl-01.example.net
Contact: <sip:localhost:8443;transport=ssl>
X-max-leg-no: 6
X-drop-with-leader: false
X-rss-id:
X-Sticky: 1
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1595912438
X-Conf_no: 9717
X-restrict-participants: false
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.mz@example.com
X-domain: DEFAULT
X-client-address: 192.0.2.6
X-From: user.mz@example.com
X-legs-on-server: 106
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
X-subject: User Four's Meeting Room
X-project-code: None
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDEuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 1171
Content-type: application/sdp

v=0
o=user.mz@example.com 873988796 127 IN IP4 uk-ed-thames-01.example.net
s=User Four's Meeting Room
i="User%20Seventeen" (+61555000002) CallByPhone-URL
c=TN RFC2543 +61555000002
t=3804901154 0
m=audio 1 voice -
a=status:(203) answered
a=mediastatus:sendrecv
a=leg_no:3
a=remote:198.51.100.71:12120
a=flags:1181218
a=new_change
a=server:hk-ed-pearl-01.example.net
a=clir:false:61255500013
a=far_end:198.51.100.66:13198
a=acs_guid:000446823944
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.mz@example.com
a=legCallid:11207077@198.51.100.71
a=audio_payload:PCMU
a=cdr_start_time:1595912354.404381
a=preferred_identity:61255500013
a=x-ring2-callowner-login_alias:user.mz@example.com
a=x-ring2-legowner-login_alias:user.mz@example.com
a=x-ring2-call_source_type:0
a=x-ring2-displayname:User%20Seventeen
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:192.0.2.3:60852
a=x-ring2-unique_device_id:00000014-0000-4000-8000-000000000000
a=x-ring2-UF1:None
a=ivr:dialout-confirm
a=access_code:00000002
a=privs:participant
a=dialout:click_in
a=x-ring2-smartproxy:usdc2-asalt04.example.net
a=user-agent:Cisco-SIPGateway/IOS-15.5.2.S4


2020-07-28 05:00:38,477070
NOTIFY sip:user.mz@example.com;pool=uk-ed-thames;box=ukdc1-edm13.example.net SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__user.mz@example.com;received=127.0.0.1:34862:1442942896
Via: SIP/2.0/TCP 127.0.0.1:48868;received=127.0.0.1:48868:26772976
Call-ID: call-id-0001.00000000000000server01.example.net
CSeq: 15 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-thames;box=ukdc1-edm13.example.net
To: sip:user.mz@example.com;pool=uk-ed-thames;box=ukdc1-edm13.example.net
Contact: <sip:localhost:8443;transport=ssl>
X-max-leg-no: 6
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1595912438
X-drop-with-leader: false
X-Conf_no: 9717
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.mz@example.com
X-domain: DEFAULT
X-client-address: 127.0.0.1
X-From: user.mz@example.com
X-legs-on-server: 5
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
X-Billing-code-required: false
X-recording-enabled: true
X-leader-required: true
X-Video-UsingMCU: false
X-subject: User Four's Meeting Room
X-project-code: None
X-reserved-ports: 0
X-restrict-participants: false
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDEuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 775
Content-type: application/sdp

v=0
o=user.mz@example.com 57046985175711 847315521 IN IP4 uk-ed-thames-01.example.net
s=
i=+44555000000001 (+44555000000001) CallByPhone-URL
c=TN RFC2543 +44555000000001
t=3804900991 0
m=audio 1 voice -
a=status:(203) answered
a=mediastatus:sendrecv
a=leg_no:1
a=remote:203.0.113.41:12002
a=flags:1049634
a=new_change
a=server:ukdc1-edm13.example.net
a=clir:false
a=far_end:10.44.177.35:38880
a=acs_guid:000471698977
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:+44044987799@203.0.113.41
a=audio_payload:PCMU
a=cdr_start_time:1595912191.862391
a=access_code:00000002
a=privs:participant
a=ivr:dialin
a=creator:user.mz@example.com
a=dialin:00000002:6660013449@10.44.177.35-$$-1
a=dest_phone:+44044987799
a=user-agent:Cisco-SIPGateway/IOS-16.9.4


2020-07-28 05:00:38,477931
NOTIFY sip:user.mz@example.com;pool=uk-ed-thames;box=ukdc1-edm13.example.net SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__user.mz@example.com;received=127.0.0.1:34862:1442942896
Via: SIP/2.0/TCP 127.0.0.1:48868;received=127.0.0.1:48868:26772976
Call-ID: call-id-0001.00000000000000server01.example.net
CSeq: 16 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-thames;box=ukdc1-edm13.example.net
To: sip:user.mz@example.com;pool=uk-ed-thames;box=ukdc1-edm13.example.net
Contact: <sip:localhost:8443;transport=ssl>
X-max-leg-no: 6
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1595912438
X-drop-with-leader: false
X-Conf_no: 9717
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.mz@example.com
X-domain: DEFAULT
X-client-address: 127.0.0.1
X-From: user.mz@example.com
X-legs-on-server: 5
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
X-Billing-code-required: false
X-recording-enabled: true
X-leader-required: true
X-Video-UsingMCU: false
X-subject: User Four's Meeting Room
X-project-code: None
X-reserved-ports: 0
X-restrict-participants: false
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDEuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 775
Content-type: application/sdp

v=0
o=user.mz@example.com 57176408823365 847315538 IN IP4 uk-ed-thames-01.example.net
s=
i=+44555000000002 (+44555000000002) CallByPhone-URL
c=TN RFC2543 +44555000000002
t=3804900978 0
m=audio 1 voice -
a=status:(203) answered
a=mediastatus:sendrecv
a=leg_no:2
a=remote:203.0.113.41:12000
a=flags:1049634
a=new_change
a=server:ukdc1-edm13.example.net
a=clir:false
a=far_end:10.44.177.35:42752
a=acs_guid:000471698977
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:+44044987799@203.0.113.41
a=audio_payload:PCMU
a=cdr_start_time:1595912178.258632
a=access_code:00000002
a=privs:participant
a=ivr:dialin
a=creator:user.mz@example.com
a=dialin:00000002:6660013448@10.44.177.35-$$-1
a=dest_phone:+44044987799
a=user-agent:Cisco-SIPGateway/IOS-16.9.4


2020-07-28 05:05:48,583143
NOTIFY sip:user.mz@example.com;pool=uk-ed-thames;box=ukdc1-edm13.example.net SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__user.mz@example.com;received=127.0.0.1:34862:1442942896
Via: SIP/2.0/TCP 127.0.0.1:48868;received=127.0.0.1:48868:26772976
Call-ID: call-id-0001.00000000000000server01.example.net
CSeq: 51 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-thames;box=ukdc1-edm13.example.net
To: sip:user.mz@example.com;pool=uk-ed-thames;box=ukdc1-edm13.example.net
Contact: <sip:localhost:8443;transport=ssl>
X-max-leg-no: 7
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1595912748
X-drop-with-leader: false
X-Conf_no: 9717
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.mz@example.com
X-domain: DEFAULT
X-client-address: 127.0.0.1
X-From: user.mz@example.com
X-legs-on-server: 14
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
X-Billing-code-required: false
X-recording-enabled: true
X-leader-required: true
X-Video-UsingMCU: false
X-subject: User Four's Meeting Room
X-project-code: None
X-reserved-ports: 0
X-restrict-participants: false
X-active-talker:
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDEuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 1711
Content-type: application/sdp

v=0
o=user.mz@example.com 57046985175711 847315547 IN IP4 uk-ed-thames-01.example.net
s=
i=+44555000000001 (+44555000000001) CallByPhone-URL
c=TN RFC2543 +44555000000001
t=3804900991 0
m=audio 1 voice -
a=status:(203) answered
a=mediastatus:sendrecv
a=leg_no:1
a=remote:203.0.113.41:12002
a=flags:1049634
a=new_change
a=server:ukdc1-edm13.example.net
a=clir:false
a=far_end:10.44.177.35:38880
a=acs_guid:000471698977
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:+44044987799@203.0.113.41
a=audio_payload:PCMU
a=cdr_start_time:1595912191.862391
a=access_code:00000002
a=privs:participant
a=ivr:dialin
a=creator:user.mz@example.com
a=dialin:00000002:6660013449@10.44.177.35-$$-1
a=dest_phone:+44044987799
a=user-agent:Cisco-SIPGateway/IOS-16.9.4
v=0
o=user.mz@example.com 874381890 874381994 IN IP4 uk-ed-thames-01.example.net
s=call-id-0001.00000000000000server01.example.net
i=Recording by user.mz@example.com (call-id-0001.00000000000000server01.example.net) CallByPhone-URL
e=13773497
c=IN IP4 127.0.0.1
t=3804901547 0
m=audio 1 voice x-record
a=status:(1) pending
a=mediastatus:recvonly
a=leg_no:7
a=flags:1049120
a=xflags:1
a=new_change
a=server:ukdc1-edm13.example.net
a=clir:false
a=fmtp:x-record uri:file:4D4463774D6A67344D6A55334F5451304E4455794E6A49354E5335316179316C5A433130614746745A584D744D444575636D6C755A7A4975593239744F6A45314F5455354D5449794D4459364E7A4D774D6A5532/recording.wav;host=10.254.254.1;access_code=13773497
a=machine_leg:1
a=acs_guid:000471698977
a=x-ring2-call_source_type:0
a=x-ring2-smartproxy:usdc2-asalt04.example.net
a=x-ring2-unique_device_id:00000006-0000-4000-8000-000000000000
a=x-ring2-clientaddress:203.0.113.56:54290


2020-07-28 05:05:56,455385
NOTIFY sip:user.mz@example.com;pool=uk-ed-thames;box=ukdc1-edm13.example.net SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__user.mz@example.com;received=127.0.0.1:34862:1442942896
Via: SIP/2.0/TCP 127.0.0.1:48868;received=127.0.0.1:48868:26772976
Call-ID: call-id-0001.00000000000000server01.example.net
CSeq: 52 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-thames;box=ukdc1-edm13.example.net
To: sip:user.mz@example.com;pool=uk-ed-thames;box=ukdc1-edm13.example.net
Contact: <sip:localhost:8443;transport=ssl>
X-max-leg-no: 7
X-CallEvent: RecordingStart
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1595912756
X-drop-with-leader: false
X-Conf_no: 9717
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.mz@example.com
X-domain: DEFAULT
X-client-address: 127.0.0.1
X-From: user.mz@example.com
X-legs-on-server: 14
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
X-Billing-code-required: false
X-recording-enabled: true
X-leader-required: true
X-Video-UsingMCU: false
X-subject: User Four's Meeting Room
X-project-code: None
X-reserved-ports: 0
X-restrict-participants: false
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDEuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 1055
Content-type: application/sdp

v=0
o=user.mz@example.com 874381890 874382005 IN IP4 uk-ed-thames-01.example.net
s=call-id-0001.00000000000000server01.example.net
i=Recording by user.mz@example.com (call-id-0001.00000000000000server01.example.net) CallByPhone-URL
e=13773497
c=IN IP4 127.0.0.1
t=3804901547 0
m=audio 1 voice x-record
a=status:(203) answered recording
a=mediastatus:recvonly
a=leg_no:7
a=flags:1049120
a=xflags:1
a=new_change
a=server:ukdc1-edm13.example.net
a=clir:false
a=fmtp:x-record uri:file:4D4463774D6A67344D6A55334F5451304E4455794E6A49354E5335316179316C5A433130614746745A584D744D444575636D6C755A7A4975593239744F6A45314F5455354D5449794D4459364E7A4D774D6A5532/recording.wav;host=10.254.254.1;access_code=13773497
a=machine_leg:1
a=acs_guid:000471698977
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.mz@example.com
a=cdr_start_time:1595912747.386672
a=x-ring2-call_source_type:0
a=x-ring2-smartproxy:usdc2-asalt04.example.net
a=x-ring2-unique_device_id:00000006-0000-4000-8000-000000000000
a=x-ring2-clientaddress:203.0.113.56:54290


2020-07-28 05:20:46,128657
NOTIFY sip:user.mz@example.com;pool=hk-ed-pearl;box=hk-ed-pearl-01.example.net SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__user.mz@example.com;received=127.0.0.1:45930:1442942896
Via: SIP/2.0/TCP 127.0.0.1:38454;received=127.0.0.1:38454:20705328
Call-ID: call-id-0001.00000000000000server01.example.net
CSeq: 107 NOTIFY
From: sip:conference.wizard@local.host;pool=hk-ed-pearl;box=hk-ed-pearl-01.example.net
To: sip:user.mz@example.com;pool=hk-ed-pearl;box=hk-ed-pearl-01.example.net
Contact: <sip:localhost:8443;transport=ssl>
X-max-leg-no: 7
X-CallEvent: LegDrop
X-drop-with-leader: false
X-rss-id:
X-Sticky: 1
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1595913646
X-Conf_no: 9717
X-restrict-participants: false
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.mz@example.com
X-domain: DEFAULT
X-client-address: 192.0.2.6
X-From: user.mz@example.com
X-legs-on-server: 88
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
X-subject: User Four's Meeting Room
X-project-code: None
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDEuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 1292
Content-type: application/sdp

v=0
o=user.mz@example.com 873988796 140 IN IP4 uk-ed-thames-01.example.net
s=User Four's Meeting Room
i="User%20Seventeen" (+61555000002) CallByPhone-URL
c=TN RFC2543 +61555000002
t=3804901154 3804902446
m=audio 1 voice -
a=status:(4) dropped
a=mediastatus:sendrecv
a=cause: (20001) Normal Hangup - Received SIP BYE From: +61555000002@192.0.2.5
a=leg_no:3
a=remote:198.51.100.71:12120
a=flags:1181218
a=new_change
a=server:hk-ed-pearl-01.example.net
a=clir:false:61255500013
a=far_end:198.51.100.66:13198
a=acs_guid:000446823944
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.mz@example.com
a=legCallid:11207077@198.51.100.71
a=audio_payload:PCMU
a=cdr_start_time:1595912354.404381
a=preferred_identity:61255500013
a=x-ring2-callowner-login_alias:user.mz@example.com
a=x-ring2-legowner-login_alias:user.mz@example.com
a=x-ring2-call_source_type:0
a=x-ring2-displayname:User%20Seventeen
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:192.0.2.3:60852
a=x-ring2-unique_device_id:00000014-0000-4000-8000-000000000000
a=x-ring2-UF1:None
a=ivr:dialout-confirm
a=access_code:00000002
a=privs:participant
a=dialout:click_in
a=x-ring2-smartproxy:usdc2-asalt04.example.net
a=user-agent:Cisco-SIPGateway/IOS-15.5.2.S4
a=cdr_stop_time:1595913646.127538


2020-07-28 05:20:47,817452
NOTIFY sip:user.mz@example.com;pool=uk-ed-thames;box=ukdc1-edm13.example.net SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__user.mz@example.com;received=127.0.0.1:34862:1442942896
Via: SIP/2.0/TCP 127.0.0.1:48868;received=127.0.0.1:48868:26772976
Call-ID: call-id-0001.00000000000000server01.example.net
CSeq: 115 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-thames;box=ukdc1-edm13.example.net
To: sip:user.mz@example.com;pool=uk-ed-thames;box=ukdc1-edm13.example.net
Contact: <sip:localhost:8443;transport=ssl>
X-max-leg-no: 7
X-CallEvent: LegDrop
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1595913647
X-drop-with-leader: false
X-Conf_no: 9717
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.mz@example.com
X-domain: DEFAULT
X-client-address: 127.0.0.1
X-From: user.mz@example.com
X-legs-on-server: 10
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
X-Billing-code-required: false
X-recording-enabled: true
X-leader-required: true
X-Video-UsingMCU: false
X-subject: User Four's Meeting Room
X-project-code: None
X-reserved-ports: 0
X-restrict-participants: false
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDEuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 902
Content-type: application/sdp

v=0
o=user.mz@example.com 57176408823365 847315557 IN IP4 uk-ed-thames-01.example.net
s=
i=+44555000000002 (+44555000000002) CallByPhone-URL
c=TN RFC2543 +44555000000002
t=3804900978 3804902447
m=audio 1 voice -
a=status:(4) dropped
a=mediastatus:sendrecv
a=cause: (20001) Normal Hangup - Received SIP BYE From: +44555000000002@10.44.177.35
a=leg_no:2
a=remote:203.0.113.41:12000
a=flags:1049634
a=new_change
a=server:ukdc1-edm13.example.net
a=clir:false
a=far_end:10.44.177.35:42752
a=acs_guid:000471698977
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:+44044987799@203.0.113.41
a=audio_payload:PCMU
a=cdr_start_time:1595912178.258632
a=access_code:00000002
a=privs:participant
a=ivr:dialin
a=creator:user.mz@example.com
a=dialin:00000002:6660013448@10.44.177.35-$$-1
a=dest_phone:+44044987799
a=user-agent:Cisco-SIPGateway/IOS-16.9.4
a=cdr_stop_time:1595913647.816589


2020-07-28 05:20:49,543072
NOTIFY sip:user.mz@example.com;pool=hk-ed-pearl;box=hk-ed-pearl-01.example.net SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__user.mz@example.com;received=127.0.0.1:45930:1442942896
Via: SIP/2.0/TCP 127.0.0.1:38454;received=127.0.0.1:38454:20705328
Call-ID: call-id-0001.00000000000000server01.example.net
CSeq: 109 NOTIFY
From: sip:conference.wizard@local.host;pool=hk-ed-pearl;box=hk-ed-pearl-01.example.net
To: sip:user.mz@example.com;pool=hk-ed-pearl;box=hk-ed-pearl-01.example.net
Contact: <sip:localhost:8443;transport=ssl>
X-max-leg-no: 7
X-CallEvent: LegDrop
X-drop-with-leader: false
X-rss-id:
X-Sticky: 1
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1595913649
X-Conf_no: 9717
X-restrict-participants: false
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.mz@example.com
X-domain: DEFAULT
X-client-address: 192.0.2.6
X-From: user.mz@example.com
X-legs-on-server: 87
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
X-subject: User Four's Meeting Room
X-project-code: None
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDEuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 1305
Content-type: application/sdp

v=0
o=user.mz@example.com 874059406 230 IN IP4 uk-ed-thames-01.example.net
s=User Four's Meeting Room
i="User%20Four" (+61555000001) CallByPhone-URL
c=TN RFC2543 +61555000001
t=3804901224 3804902449
m=audio 1 voice -
a=status:(4) dropped
a=mediastatus:sendrecv
a=cause: (20001) Normal Hangup - Received SIP BYE From: +61555000001@192.0.2.5
a=leg_no:6
a=remote:198.51.100.71:12150
a=flags:1185314
a=new_change
a=server:hk-ed-pearl-01.example.net
a=clir:false:61255500013
a=far_end:198.51.100.66:12246
a=acs_guid:000446823944
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.mz@example.com
a=legCallid:11207097@198.51.100.71
a=audio_payload:PCMU
a=cdr_start_time:1595912424.968570
a=preferred_identity:61255500013
a=x-ring2-callowner-login_alias:user.mz@example.com
a=x-ring2-legowner-login_alias:user.mz@example.com
a=x-ring2-alt-proj:None
a=x-ring2-call_source_type:0
a=x-ring2-displayname:User%20Four
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:203.0.113.56:54290
a=x-ring2-unique_device_id:00000006-0000-4000-8000-000000000000
a=x-ring2-UF1:None
a=ivr:dialout-confirm
a=dialout:click_in
a=access_code:00000001
a=privs:leader
a=x-ring2-smartproxy:usdc2-asalt04.example.net
a=user-agent:Cisco-SIPGateway/IOS-15.5.2.S4
a=cdr_stop_time:1595913649.542113


2020-07-28 05:20:49,586477
NOTIFY sip:user.mz@example.com;pool=uk-ed-thames;box=ukdc1-edm13.example.net SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__user.mz@example.com;received=127.0.0.1:34862:1442942896
Via: SIP/2.0/TCP 127.0.0.1:48868;received=127.0.0.1:48868:26772976
Call-ID: call-id-0001.00000000000000server01.example.net
CSeq: 116 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-thames;box=ukdc1-edm13.example.net
To: sip:user.mz@example.com;pool=uk-ed-thames;box=ukdc1-edm13.example.net
Contact: <sip:localhost:8443;transport=ssl>
X-max-leg-no: 7
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1595913649
X-drop-with-leader: false
X-Conf_no: 9717
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.mz@example.com
X-domain: DEFAULT
X-client-address: 127.0.0.1
X-From: user.mz@example.com
X-legs-on-server: 9
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
X-Billing-code-required: false
X-recording-enabled: true
X-leader-required: true
X-Video-UsingMCU: false
X-subject: User Four's Meeting Room
X-project-code: None
X-reserved-ports: 0
X-restrict-participants: false
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDEuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 1862
Content-type: application/sdp

v=0
o=user.mz@example.com 57046985175711 847315620 IN IP4 uk-ed-thames-01.example.net
s=
i=+44555000000001 (+44555000000001) CallByPhone-URL
c=TN RFC2543 +44555000000001
t=3804900991 3804902449
m=audio 1 voice -
a=status:(4) dropped
a=mediastatus:sendrecv
a=cause: (20001) Normal Hangup - Received SIP BYE From: +44555000000001@10.44.177.35
a=leg_no:1
a=remote:203.0.113.41:12002
a=flags:1049634
a=new_change
a=server:ukdc1-edm13.example.net
a=clir:false
a=far_end:10.44.177.35:38880
a=acs_guid:000471698977
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:+44044987799@203.0.113.41
a=audio_payload:PCMU
a=cdr_start_time:1595912191.862391
a=access_code:00000002
a=privs:participant
a=ivr:dialin
a=creator:user.mz@example.com
a=dialin:00000002:6660013449@10.44.177.35-$$-1
a=dest_phone:+44044987799
a=user-agent:Cisco-SIPGateway/IOS-16.9.4
a=cdr_stop_time:1595913649.585872
v=0
o=user.mz@example.com 1804662462 847315690 IN IP4 localhost
s=
i=usecallid-leg-4 (usecallid-leg-4) CallByPhone
c=IN IP4 198.51.100.71
t=3804901154 3804902449
m=audio 8766 RTP/AVP 0 101
a=status:(12) ending
a=mediastatus:sendrecv
a=cause: (40027) THS_CAUSE_DROP_CONNECTOR
a=leg_no:4
a=remote:203.0.113.41:12008
a=flags:1049090
a=new_change
a=server:ukdc1-edm13.example.net
a=sipphone:usecallid_call-id-0001.00000000000000server01.example.net@198.51.100.71;port=5060
a=clir:false
a=fmtp:101 0-15
a=far_end:198.51.100.71:12126
a=acs_guid:000471698977
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.mz@example.com
a=legCallid:1909584670@203.0.113.41
a=audio_payload:PCMU
a=cdr_start_time:1595912354.645103
a=rtpmap:0 pcmu/8000/1
a=rtpmap:101 telephone-event/8000
a=dial_once:hk-ed-pearl-01.example.net
a=access_code:0000000
a=privs:participant
a=dialout:click_in
a=ivr:dialout
a=user-agent:Example eDial ACS 9.1.0b8071


2020-07-28 05:20:49,589220
NOTIFY sip:user.mz@example.com;pool=uk-ed-thames;box=ukdc1-edm13.example.net SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__user.mz@example.com;received=127.0.0.1:34862:1442942896
Via: SIP/2.0/TCP 127.0.0.1:48868;received=127.0.0.1:48868:26772976
Call-ID: call-id-0001.00000000000000server01.example.net
CSeq: 118 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-thames;box=ukdc1-edm13.example.net
To: sip:user.mz@example.com;pool=uk-ed-thames;box=ukdc1-edm13.example.net
Contact: <sip:localhost:8443;transport=ssl>
X-max-leg-no: 7
X-CallEvent: RecordingReady
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1595913649
X-drop-with-leader: false
X-Conf_no: 9717
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.mz@example.com
X-domain: DEFAULT
X-client-address: 127.0.0.1
X-From: user.mz@example.com
X-legs-on-server: 7
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
X-Billing-code-required: false
X-recording-enabled: true
X-leader-required: true
X-Video-UsingMCU: false
X-subject: User Four's Meeting Room
X-project-code: None
X-reserved-ports: 0
X-restrict-participants: false
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDEuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 1133
Content-type: application/sdp

v=0
o=user.mz@example.com 874381890 874382016 IN IP4 uk-ed-thames-01.example.net
s=call-id-0001.00000000000000server01.example.net
i=Recording by user.mz@example.com (call-id-0001.00000000000000server01.example.net) CallByPhone-URL
e=13773497
c=IN IP4 127.0.0.1
t=3804901547 3804902449
m=audio 1 voice x-record
a=status:(4) dropped
a=mediastatus:recvonly
a=cause: (20019) Conference is being closed
a=leg_no:7
a=flags:1049122
a=xflags:1
a=new_change
a=server:ukdc1-edm13.example.net
a=clir:false
a=fmtp:x-record uri:file:4D4463774D6A67344D6A55334F5451304E4455794E6A49354E5335316179316C5A433130614746745A584D744D444575636D6C755A7A4975593239744F6A45314F5455354D5449794D4459364E7A4D774D6A5532/recording.wav;host=10.254.254.1;access_code=13773497
a=machine_leg:1
a=acs_guid:000471698977
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.mz@example.com
a=cdr_start_time:1595912747.386672
a=x-ring2-call_source_type:0
a=x-ring2-smartproxy:usdc2-asalt04.example.net
a=x-ring2-unique_device_id:00000006-0000-4000-8000-000000000000
a=x-ring2-clientaddress:203.0.113.56:54290
a=cdr_stop_time:1595913649.588482
