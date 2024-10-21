2021-01-16 19:19:35,807356
NOTIFY sip:lab.edial.rc.116.voip@loopup.co;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__lab.edial.rc.116.voip@loopup.co;received=127.0.0.1:48114:442357920
Via: SIP/2.0/TCP 127.0.0.1:32900;received=127.0.0.1:32900:21527168
Call-ID: 9355232592588793289872.uk-ed-nelson-01.ring2-corp.com
CSeq: 1 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com
To: sip:lab.edial.rc.116.voip@loopup.co;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: CallStart
X-slave-site: localhost
X-last-change: 1610824775
X-Conf_no: 711
X-rollcall: disabled
X-Call-URL: true
X-control-master: lab.edial.rc.116.voip@loopup.co
X-domain: DEFAULT
X-client-address: 10.1.224.108
X-From: lab.edial.rc.116.voip@loopup.co
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
X-leader-required: true
X-Video-UsingMCU: false
X-subject: Voip User's Meeting Room
X-project-code: df
X-Call-Instance-ID: OTM1NTIzMjU5MjU4ODc5MzI4OTg3Mi51ay1lZC1uZWxzb24tMDEucmluZzItY29ycC5jb206MTYxMDgyNDc3NTo1NTcyMjc=
X-Route-ID: 1
Content-Length: 1089
Content-type: application/sdp

v=0
o=lab.edial.rc.116.voip@loopup.co 227247312 104 IN IP4 uk-ed-nelson-01.ring2-corp.com
s=Voip User's Meeting Room
i="Voip%20User" (+19172084495) CallByPhone-URL
c=TN RFC2543 +19172084495
t=3819813575 0
m=audio 1 voice -
a=status:(2) starting
a=mediastatus:sendrecv
a=leg_no:1
a=flags:1054240
a=new_change
a=server:uk-ed-nelson-04.ring2-corp.com
a=clir:false:13127560923
a=acs_guid:000006832408
a=cli-screening:00
a=useforfrom:lab.edial.rc.116.voip@loopup.co
a=legCallid:170540458@67.131.238.74
a=cdr_start_time:1610824775.753977
a=preferred_identity:13127560923
a=x-ring2-callowner-login_alias:lab.edial.rc.116.voip@loopup.co
a=x-ring2-legowner-login_alias:lab.edial.rc.116.voip@loopup.co
a=x-ring2-alt-proj:df
a=x-ring2-call_source_type:0
a=x-ring2-displayname:Voip%20User
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:135.180.107.115:48852
a=x-ring2-unique_device_id:d40835f7-2f7f-451f-9920-8afcb0f09a95
a=x-ring2-UF1:df
a=ivr:dialout
a=dialout:click_in
a=access_code:1218771
a=privs:leader
a=x-ring2-smartproxy:USAZW2-LALTAIR1.ring2-corp.com


2021-01-16 19:19:50,584632
NOTIFY sip:lab.edial.rc.116.voip@loopup.co;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__lab.edial.rc.116.voip@loopup.co;received=127.0.0.1:48114:442357920
Via: SIP/2.0/TCP 127.0.0.1:32900;received=127.0.0.1:32900:21527168
Call-ID: 9355232592588793289872.uk-ed-nelson-01.ring2-corp.com
CSeq: 3 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com
To: sip:lab.edial.rc.116.voip@loopup.co;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: LegAdd
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1610824790
X-Conf_no: 711
X-rollcall: disabled
X-Call-URL: true
X-control-master: lab.edial.rc.116.voip@loopup.co
X-domain: DEFAULT
X-client-address: 10.1.224.108
X-From: lab.edial.rc.116.voip@loopup.co
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
X-leader-required: true
X-Video-UsingMCU: false
X-subject: Voip User's Meeting Room
X-project-code: df
X-Call-Instance-ID: OTM1NTIzMjU5MjU4ODc5MzI4OTg3Mi51ay1lZC1uZWxzb24tMDEucmluZzItY29ycC5jb206MTYxMDgyNDc3NTo1NTcyMjc=
X-Route-ID: 1
Content-Length: 1216
Content-type: application/sdp

v=0
o=lab.edial.rc.116.voip@loopup.co 227247312 115 IN IP4 uk-ed-nelson-01.ring2-corp.com
s=Voip User's Meeting Room
i="Voip%20User" (+19172084495) CallByPhone-URL
c=TN RFC2543 +19172084495
t=3819813590 0
m=audio 1 voice -
a=status:(203) answered  
a=mediastatus:sendrecv
a=leg_no:1
a=remote:67.131.238.74:12000
a=flags:1054240
a=new_change
a=server:uk-ed-nelson-04.ring2-corp.com
a=clir:false:13127560923
a=far_end:216.111.92.72:18374
a=acs_guid:000006832408
a=cli-screening:00
a=useforfrom:lab.edial.rc.116.voip@loopup.co
a=legCallid:170540458@67.131.238.74
a=audio_payload:PCMU
a=cdr_start_time:1610824775.753977
a=preferred_identity:13127560923
a=x-ring2-callowner-login_alias:lab.edial.rc.116.voip@loopup.co
a=x-ring2-legowner-login_alias:lab.edial.rc.116.voip@loopup.co
a=x-ring2-alt-proj:df
a=x-ring2-call_source_type:0
a=x-ring2-displayname:Voip%20User
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:135.180.107.115:48852
a=x-ring2-unique_device_id:d40835f7-2f7f-451f-9920-8afcb0f09a95
a=x-ring2-UF1:df
a=ivr:dialout
a=dialout:click_in
a=access_code:1218771
a=privs:leader
a=x-ring2-smartproxy:USAZW2-LALTAIR1.ring2-corp.com
a=user-agent:Cisco-SIPGateway/IOS-12.x


2021-01-16 19:20:24,339283
NOTIFY sip:lab.edial.rc.116.voip@loopup.co;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__lab.edial.rc.116.voip@loopup.co;received=127.0.0.1:48114:442357920
Via: SIP/2.0/TCP 127.0.0.1:32900;received=127.0.0.1:32900:21527168
Call-ID: 9355232592588793289872.uk-ed-nelson-01.ring2-corp.com
CSeq: 8 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com
To: sip:lab.edial.rc.116.voip@loopup.co;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com
Contact: <sip:localhost:8443;transport=ssl>
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1610824824
X-Conf_no: 711
X-rollcall: disabled
X-Call-URL: true
X-control-master: lab.edial.rc.116.voip@loopup.co
X-domain: DEFAULT
X-client-address: 10.1.224.108
X-From: lab.edial.rc.116.voip@loopup.co
X-legs-on-server: 2
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
X-subject: Voip User's Meeting Room
X-project-code: df
X-active-talker: true
X-Call-Instance-ID: OTM1NTIzMjU5MjU4ODc5MzI4OTg3Mi51ay1lZC1uZWxzb24tMDEucmluZzItY29ycC5jb206MTYxMDgyNDc3NTo1NTcyMjc=
X-Route-ID: 1
Content-Length: 2054
Content-type: application/sdp

v=0
o=lab.edial.rc.116.voip@loopup.co 227247312 130 IN IP4 uk-ed-nelson-01.ring2-corp.com
s=Voip User's Meeting Room
i="Voip%20User" (+19172084495) CallByPhone-URL
c=TN RFC2543 +19172084495
t=3819813590 0
m=audio 1 voice -
a=status:(203) answered  
a=mediastatus:sendrecv
a=leg_no:1
a=remote:67.131.238.74:12000
a=flags:1054242
a=new_change
a=server:uk-ed-nelson-04.ring2-corp.com
a=clir:false:13127560923
a=far_end:216.111.92.72:18374
a=acs_guid:000006832408
a=cli-screening:00
a=useforfrom:lab.edial.rc.116.voip@loopup.co
a=legCallid:170540458@67.131.238.74
a=audio_payload:PCMU
a=cdr_start_time:1610824775.753977
a=preferred_identity:13127560923
a=x-ring2-callowner-login_alias:lab.edial.rc.116.voip@loopup.co
a=x-ring2-legowner-login_alias:lab.edial.rc.116.voip@loopup.co
a=x-ring2-alt-proj:df
a=x-ring2-call_source_type:0
a=x-ring2-displayname:Voip%20User
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:135.180.107.115:48852
a=x-ring2-unique_device_id:d40835f7-2f7f-451f-9920-8afcb0f09a95
a=x-ring2-UF1:df
a=ivr:dialout
a=dialout:click_in
a=access_code:1218771
a=privs:leader
a=x-ring2-smartproxy:USAZW2-LALTAIR1.ring2-corp.com
a=user-agent:Cisco-SIPGateway/IOS-12.x
v=0
o=lab.edial.rc.116.voip@loopup.co 227296328 104 IN IP4 uk-ed-nelson-01.ring2-corp.com
s=Voip User's Meeting Room
i="Heathrow" (+448443351801) CallByPhone-URL
c=TN RFC2543 +448443351801
t=3819813624 0
m=audio 1 voice -
a=status:(1) pending
a=leg_no:2
a=flags:1050144
a=new_change
a=server:uk-ed-nelson-04.ring2-corp.com
a=clir:false:13127560923
a=acs_guid:000006832408
a=preferred_identity:13127560923
a=x-ring2-callowner-login_alias:lab.edial.rc.116.voip@loopup.co
a=x-ring2-legowner-login_alias:lab.edial.rc.116.voip@loopup.co
a=x-ring2-call_source_type:0
a=x-ring2-displayname:Heathrow
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:135.180.107.115:48852
a=x-ring2-UF1:df
a=ivr:dialout
a=access_code:1572753
a=privs:participant
a=dialout:click_in
a=x-ring2-smartproxy:USAZW2-LALTAIR1.ring2-corp.com


2021-01-16 19:20:24,378624
NOTIFY sip:lab.edial.rc.116.voip@loopup.co;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__lab.edial.rc.116.voip@loopup.co;received=127.0.0.1:48114:442357920
Via: SIP/2.0/TCP 127.0.0.1:32900;received=127.0.0.1:32900:21527168
Call-ID: 9355232592588793289872.uk-ed-nelson-01.ring2-corp.com
CSeq: 9 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com
To: sip:lab.edial.rc.116.voip@loopup.co;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com
Contact: <sip:localhost:8443;transport=ssl>
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1610824824
X-Conf_no: 711
X-rollcall: disabled
X-Call-URL: true
X-control-master: lab.edial.rc.116.voip@loopup.co
X-domain: DEFAULT
X-client-address: 10.1.224.108
X-From: lab.edial.rc.116.voip@loopup.co
X-legs-on-server: 2
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
X-subject: Voip User's Meeting Room
X-project-code: df
X-Call-Instance-ID: OTM1NTIzMjU5MjU4ODc5MzI4OTg3Mi51ay1lZC1uZWxzb24tMDEucmluZzItY29ycC5jb206MTYxMDgyNDc3NTo1NTcyMjc=
X-Route-ID: 1
Content-Length: 1002
Content-type: application/sdp

v=0
o=lab.edial.rc.116.voip@loopup.co 227296328 105 IN IP4 uk-ed-nelson-01.ring2-corp.com
s=Voip User's Meeting Room
i="Heathrow" (+448443351801) CallByPhone-URL
c=TN RFC2543 +448443351801
t=3819813624 0
m=audio 1 voice -
a=status:(2) starting
a=mediastatus:sendrecv
a=leg_no:2
a=flags:1050144
a=new_change
a=server:uk-ed-nelson-04.ring2-corp.com
a=clir:false:13127560923
a=acs_guid:000006832408
a=cli-screening:00
a=useforfrom:lab.edial.rc.116.voip@loopup.co
a=legCallid:170540459@67.131.238.74
a=cdr_start_time:1610824824.294101
a=preferred_identity:13127560923
a=x-ring2-callowner-login_alias:lab.edial.rc.116.voip@loopup.co
a=x-ring2-legowner-login_alias:lab.edial.rc.116.voip@loopup.co
a=x-ring2-call_source_type:0
a=x-ring2-displayname:Heathrow
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:135.180.107.115:48852
a=x-ring2-UF1:df
a=ivr:dialout
a=access_code:1572753
a=privs:participant
a=dialout:click_in
a=x-ring2-smartproxy:USAZW2-LALTAIR1.ring2-corp.com


2021-01-16 19:20:38,515333
NOTIFY sip:lab.edial.rc.116.voip@loopup.co;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__lab.edial.rc.116.voip@loopup.co;received=127.0.0.1:48114:442357920
Via: SIP/2.0/TCP 127.0.0.1:32900;received=127.0.0.1:32900:21527168
Call-ID: 9355232592588793289872.uk-ed-nelson-01.ring2-corp.com
CSeq: 10 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com
To: sip:lab.edial.rc.116.voip@loopup.co;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: LegDrop
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1610824838
X-Conf_no: 711
X-rollcall: disabled
X-Call-URL: true
Warning: X-920 Unknown leg start failure
X-control-master: lab.edial.rc.116.voip@loopup.co
X-domain: DEFAULT
X-client-address: 10.1.224.108
X-From: lab.edial.rc.116.voip@loopup.co
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
X-leader-required: true
X-Video-UsingMCU: false
X-subject: Voip User's Meeting Room
X-project-code: df
X-Call-Instance-ID: OTM1NTIzMjU5MjU4ODc5MzI4OTg3Mi51ay1lZC1uZWxzb24tMDEucmluZzItY29ycC5jb206MTYxMDgyNDc3NTo1NTcyMjc=
X-Route-ID: 1
Content-Length: 1104
Content-type: application/sdp

v=0
o=lab.edial.rc.116.voip@loopup.co 227296328 106 IN IP4 uk-ed-nelson-01.ring2-corp.com
s=Voip User's Meeting Room
i="Heathrow" (+448443351801) CallByPhone-URL
c=TN RFC2543 +448443351801
t=3819813638 3819813638
m=audio 1 voice -
a=status:(11) failed: other (Forbidden)
a=mediastatus:sendrecv
a=leg_no:2
a=flags:1050144
a=new_change
a=server:uk-ed-nelson-04.ring2-corp.com
a=clir:false:13127560923
a=acs_guid:000006832408
a=cli-screening:00
a=useforfrom:lab.edial.rc.116.voip@loopup.co
a=legCallid:170540459@67.131.238.74
a=cdr_start_time:1610824824.294101
a=preferred_identity:13127560923
a=x-ring2-callowner-login_alias:lab.edial.rc.116.voip@loopup.co
a=x-ring2-legowner-login_alias:lab.edial.rc.116.voip@loopup.co
a=x-ring2-call_source_type:0
a=x-ring2-displayname:Heathrow
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:135.180.107.115:48852
a=x-ring2-UF1:df
a=ivr:dialout
a=access_code:1572753
a=privs:participant
a=dialout:click_in
a=x-ring2-smartproxy:USAZW2-LALTAIR1.ring2-corp.com
a=user-agent:Cisco-SIPGateway/IOS-12.x
a=cdr_stop_time:1610824838.512336


2021-01-16 19:21:58,813035
NOTIFY sip:lab.edial.rc.116.voip@loopup.co;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__lab.edial.rc.116.voip@loopup.co;received=127.0.0.1:48114:442357920
Via: SIP/2.0/TCP 127.0.0.1:32900;received=127.0.0.1:32900:21527168
Call-ID: 9355232592588793289872.uk-ed-nelson-01.ring2-corp.com
CSeq: 11 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com
To: sip:lab.edial.rc.116.voip@loopup.co;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com
Contact: <sip:localhost:8443;transport=ssl>
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1610824918
X-Conf_no: 711
X-rollcall: disabled
X-Call-URL: true
X-control-master: lab.edial.rc.116.voip@loopup.co
X-domain: DEFAULT
X-client-address: 10.1.224.108
X-From: lab.edial.rc.116.voip@loopup.co
X-legs-on-server: 2
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
X-subject: Voip User's Meeting Room
X-project-code: df
X-Call-Instance-ID: OTM1NTIzMjU5MjU4ODc5MzI4OTg3Mi51ay1lZC1uZWxzb24tMDEucmluZzItY29ycC5jb206MTYxMDgyNDc3NTo1NTcyMjc=
X-Route-ID: 1
Content-Length: 1097
Content-type: application/sdp

v=0
o=lab.edial.rc.116.voip@loopup.co 227390781 104 IN IP4 uk-ed-nelson-01.ring2-corp.com
s=Voip User's Meeting Room
i="voip%20test" (+442080163962x,7415135063) CallByPhone-URL
c=TN RFC2543 +442080163962x,7415135063
t=3819813718 0
m=audio 1 voice -
a=status:(2) starting
a=mediastatus:sendrecv
a=leg_no:3
a=flags:1050144
a=new_change
a=server:uk-ed-nelson-04.ring2-corp.com
a=clir:false:13127560923
a=acs_guid:000006832408
a=cli-screening:00
a=useforfrom:lab.edial.rc.116.voip@loopup.co
a=legCallid:170540460@67.131.238.74
a=cdr_start_time:1610824918.750885
a=preferred_identity:13127560923
a=x-ring2-callowner-login_alias:lab.edial.rc.116.voip@loopup.co
a=x-ring2-legowner-login_alias:lab.edial.rc.116.voip@loopup.co
a=x-ring2-call_source_type:0
a=x-ring2-displayname:voip%20test
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:135.180.107.115:38615
a=x-ring2-unique_device_id:4409e2a7-4317-4856-b585-562d2b2d501d
a=x-ring2-UF1:df
a=ivr:dialout
a=access_code:1572753
a=privs:participant
a=dialout:click_in
a=x-ring2-smartproxy:USAZW2-LALTAIR1.ring2-corp.com


2021-01-16 19:22:06,173761
NOTIFY sip:lab.edial.rc.116.voip@loopup.co;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__lab.edial.rc.116.voip@loopup.co;received=127.0.0.1:48114:442357920
Via: SIP/2.0/TCP 127.0.0.1:32900;received=127.0.0.1:32900:21527168
Call-ID: 9355232592588793289872.uk-ed-nelson-01.ring2-corp.com
CSeq: 12 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com
To: sip:lab.edial.rc.116.voip@loopup.co;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: LegDrop
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1610824926
X-Conf_no: 711
X-rollcall: disabled
X-Call-URL: true
X-control-master: lab.edial.rc.116.voip@loopup.co
X-domain: DEFAULT
X-client-address: 10.1.224.108
X-From: lab.edial.rc.116.voip@loopup.co
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
X-leader-required: true
X-Video-UsingMCU: false
X-subject: Voip User's Meeting Room
X-project-code: df
X-Call-Instance-ID: OTM1NTIzMjU5MjU4ODc5MzI4OTg3Mi51ay1lZC1uZWxzb24tMDEucmluZzItY29ycC5jb206MTYxMDgyNDc3NTo1NTcyMjc=
X-Route-ID: 1
Content-Length: 1349
Content-type: application/sdp

v=0
o=lab.edial.rc.116.voip@loopup.co 227390781 105 IN IP4 uk-ed-nelson-01.ring2-corp.com
s=Voip User's Meeting Room
i="voip%20test" (+442080163962x,7415135063) CallByPhone-URL
c=TN RFC2543 +442080163962x,7415135063
t=3819813726 3819813726
m=audio 1 voice -
a=status:(5) failed: busy
a=mediastatus:sendrecv
a=cause: (20003) Line Busy - Received SIP BYE From: +442080163962@216.111.92.72
a=leg_no:3
a=remote:67.131.238.74:12002
a=flags:1050144
a=new_change
a=server:uk-ed-nelson-04.ring2-corp.com
a=clir:false:13127560923
a=far_end:216.111.92.72:23866
a=acs_guid:000006832408
a=cli-screening:00
a=useforfrom:lab.edial.rc.116.voip@loopup.co
a=legCallid:170540460@67.131.238.74
a=audio_payload:PCMU
a=cdr_start_time:1610824918.750885
a=preferred_identity:13127560923
a=x-ring2-callowner-login_alias:lab.edial.rc.116.voip@loopup.co
a=x-ring2-legowner-login_alias:lab.edial.rc.116.voip@loopup.co
a=x-ring2-call_source_type:0
a=x-ring2-displayname:voip%20test
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:135.180.107.115:38615
a=x-ring2-unique_device_id:4409e2a7-4317-4856-b585-562d2b2d501d
a=x-ring2-UF1:df
a=ivr:dialout
a=access_code:1572753
a=privs:participant
a=dialout:click_in
a=x-ring2-smartproxy:USAZW2-LALTAIR1.ring2-corp.com
a=user-agent:Cisco-SIPGateway/IOS-12.x
a=cdr_stop_time:1610824926.170904
