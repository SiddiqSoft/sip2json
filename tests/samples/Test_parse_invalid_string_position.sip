2021-01-16 00:04:07,557771
NOTIFY sip:1-lab.nelson.rc141.admin@loopup.co;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com-1 SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__lab.nelson.rc141.admin@loopup.co;received=127.0.0.1:48114:442357920
Via: SIP/2.0/TCP 127.0.0.1:32900;received=127.0.0.1:32900:21527168
Call-ID: 71783810694800117894.uk-ed-nelson-01.ring2-corp.com
CSeq: 1 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com
To: sip:lab.nelson.rc141.admin@loopup.co;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: CallStart
X-slave-site: localhost
X-last-change: 1610755447
X-Conf_no: 705
X-rollcall: disabled
X-Call-URL: true
X-control-master: lab.nelson.rc141.admin@loopup.co
X-domain: DEFAULT
X-client-address: 10.1.224.108
X-From: lab.nelson.rc141.admin@loopup.co
X-legs-on-server: 1
X-start-muted: false
X-Video-SingleView: 0
X-suppress-system-im: false
X-restrict-notify: false
X-notify-im: false
X-no-audio: false
X-no-video: false
X-dialout: allowed
X-Billing-code-required: true
X-recording-enabled: true
X-leader-required: true
X-Video-UsingMCU: false
X-subject: Lab Nelson's Meeting Room
X-Call-Instance-ID: NzE3ODM4MTA2OTQ4MDAxMTc4OTQudWstZWQtbmVsc29uLTAxLnJpbmcyLWNvcnAuY29tOjE2MTA3NTU0NDc6MzA4NTc0
X-Route-ID: 1
Content-Length: 1062
Content-type: application/sdp

v=0
o=lab.nelson.rc141.admin@loopup.co 157921484 104 IN IP4 uk-ed-nelson-01.ring2-corp.com
s=Lab Nelson's Meeting Room
i="Fajar" (+19362329469) CallByPhone-URL
c=TN RFC2543 +19362329469
t=3819744247 0
m=audio 1 voice -
a=status:(2) starting
a=mediastatus:sendrecv
a=leg_no:1
a=flags:1050144
a=new_change
a=server:uk-ed-nelson-04.ring2-corp.com
a=clir:false:13127560923
a=acs_guid:001028142360
a=cli-screening:00
a=useforfrom:lab.nelson.rc141.admin@loopup.co
a=legCallid:170540432@67.131.238.74
a=cdr_start_time:1610755447.430349
a=preferred_identity:13127560923
a=x-ring2-callowner-login_alias:lab.nelson.rc141.admin@loopup.co
a=x-ring2-legowner-login_alias:lab.nelson.rc141.admin@loopup.co
a=x-ring2-call_source_type:0
a=x-ring2-displayname:Fajar
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:73.162.236.146:26366
a=x-ring2-unique_device_id:cbdbaea5-3453-43cc-ae7c-d80ce1d24ae4
a=x-ring2-UF1: 
a=ivr:dialout
a=access_code:1967518
a=privs:participant
a=dialout:click_in
a=x-ring2-smartproxy:USAZW2-LALTAIR1.ring2-corp.com


2021-01-16 00:04:17,202643
NOTIFY sip:2-lab.nelson.rc141.admin@loopup.co;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com-2 SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__lab.nelson.rc141.admin@loopup.co;received=127.0.0.1:48114:442357920
Via: SIP/2.0/TCP 127.0.0.1:32900;received=127.0.0.1:32900:21527168
Call-ID: 71783810694800117894.uk-ed-nelson-01.ring2-corp.com
CSeq: 3 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com
To: sip:lab.nelson.rc141.admin@loopup.co;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: LegAdd
X-slave-site: localhost
X-last-change: 1610755457
X-Conf_no: 705
X-rollcall: disabled
X-Call-URL: true
X-control-master: lab.nelson.rc141.admin@loopup.co
X-domain: DEFAULT
X-client-address: 10.1.224.108
X-From: lab.nelson.rc141.admin@loopup.co
X-legs-on-server: 1
X-start-muted: false
X-Video-SingleView: 0
X-suppress-system-im: false
X-restrict-notify: false
X-notify-im: false
X-no-audio: false
X-no-video: false
X-dialout: allowed
X-Billing-code-required: true
X-recording-enabled: true
X-leader-required: true
X-Video-UsingMCU: false
X-subject: Lab Nelson's Meeting Room
X-Call-Instance-ID: NzE3ODM4MTA2OTQ4MDAxMTc4OTQudWstZWQtbmVsc29uLTAxLnJpbmcyLWNvcnAuY29tOjE2MTA3NTU0NDc6MzA4NTc0
X-Route-ID: 1
Content-Length: 1189
Content-type: application/sdp

v=0
o=lab.nelson.rc141.admin@loopup.co 157921484 115 IN IP4 uk-ed-nelson-01.ring2-corp.com
s=Lab Nelson's Meeting Room
i="Fajar" (+19362329469) CallByPhone-URL
c=TN RFC2543 +19362329469
t=3819744257 0
m=audio 1 voice -
a=status:(203) answered  
a=mediastatus:sendrecv
a=leg_no:1
a=remote:67.131.238.74:12000
a=flags:1050144
a=new_change
a=server:uk-ed-nelson-04.ring2-corp.com
a=clir:false:13127560923
a=far_end:216.111.92.72:22320
a=acs_guid:001028142360
a=cli-screening:00
a=useforfrom:lab.nelson.rc141.admin@loopup.co
a=legCallid:170540432@67.131.238.74
a=audio_payload:PCMU
a=cdr_start_time:1610755447.430349
a=preferred_identity:13127560923
a=x-ring2-callowner-login_alias:lab.nelson.rc141.admin@loopup.co
a=x-ring2-legowner-login_alias:lab.nelson.rc141.admin@loopup.co
a=x-ring2-call_source_type:0
a=x-ring2-displayname:Fajar
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:73.162.236.146:26366
a=x-ring2-unique_device_id:cbdbaea5-3453-43cc-ae7c-d80ce1d24ae4
a=x-ring2-UF1: 
a=ivr:dialout
a=access_code:1967518
a=privs:participant
a=dialout:click_in
a=x-ring2-smartproxy:USAZW2-LALTAIR1.ring2-corp.com
a=user-agent:Cisco-SIPGateway/IOS-12.x


2021-01-16 00:04:17,205785
NOTIFY sip:3-lab.nelson.rc141.admin@loopup.co;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com-3 SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__lab.nelson.rc141.admin@loopup.co;received=127.0.0.1:48114:442357920
Via: SIP/2.0/TCP 127.0.0.1:32900;received=127.0.0.1:32900:21527168
Call-ID: 71783810694800117894.uk-ed-nelson-01.ring2-corp.com
CSeq: 5 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com
To: sip:lab.nelson.rc141.admin@loopup.co;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com
Contact: <sip:localhost:8443;transport=ssl>
X-slave-site: localhost
X-last-change: 1610755457
X-Conf_no: 705
X-rollcall: disabled
X-Call-URL: true
X-control-master: lab.nelson.rc141.admin@loopup.co
X-domain: DEFAULT
X-client-address: 10.1.224.108
X-From: lab.nelson.rc141.admin@loopup.co
X-legs-on-server: 1
X-start-muted: false
X-Video-SingleView: 0
X-suppress-system-im: false
X-restrict-notify: false
X-notify-im: false
X-no-audio: false
X-no-video: false
X-dialout: allowed
X-Billing-code-required: true
X-recording-enabled: true
X-leader-required: true
X-Video-UsingMCU: false
X-subject: Lab Nelson's Meeting Room
X-Call-Instance-ID: NzE3ODM4MTA2OTQ4MDAxMTc4OTQudWstZWQtbmVsc29uLTAxLnJpbmcyLWNvcnAuY29tOjE2MTA3NTU0NDc6MzA4NTc0
X-Route-ID: 1
Content-Length: 1192
Content-type: application/sdp

v=0
o=lab.nelson.rc141.admin@loopup.co 157921484 127 IN IP4 uk-ed-nelson-01.ring2-corp.com
s=Lab Nelson's Meeting Room
i="Fajar" (+19362329469) CallByPhone-URL
c=TN RFC2543 +19362329469
t=3819744257 0
m=audio 1 voice -
a=status:(205) answered hold 
a=mediastatus:nomedia
a=leg_no:1
a=remote:67.131.238.74:12000
a=flags:1050146
a=new_change
a=server:uk-ed-nelson-04.ring2-corp.com
a=clir:false:13127560923
a=far_end:216.111.92.72:22320
a=acs_guid:001028142360
a=cli-screening:00
a=useforfrom:lab.nelson.rc141.admin@loopup.co
a=legCallid:170540432@67.131.238.74
a=audio_payload:PCMU
a=cdr_start_time:1610755447.430349
a=preferred_identity:13127560923
a=x-ring2-callowner-login_alias:lab.nelson.rc141.admin@loopup.co
a=x-ring2-legowner-login_alias:lab.nelson.rc141.admin@loopup.co
a=x-ring2-call_source_type:0
a=x-ring2-displayname:Fajar
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:73.162.236.146:26366
a=x-ring2-unique_device_id:cbdbaea5-3453-43cc-ae7c-d80ce1d24ae4
a=x-ring2-UF1: 
a=ivr:dialout
a=access_code:1967518
a=privs:participant
a=dialout:click_in
a=x-ring2-smartproxy:USAZW2-LALTAIR1.ring2-corp.com
a=user-agent:Cisco-SIPGateway/IOS-12.x
