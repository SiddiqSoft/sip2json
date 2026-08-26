2021-01-16 00:04:07,557771
NOTIFY sip:1-user.admin@example.org;pool=uk-ed-nelson;box=uk-ed-nelson-04.example-corp.com-1 SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__user.admin@example.org;received=127.0.0.1:48114:442357920
Via: SIP/2.0/TCP 127.0.0.1:32900;received=127.0.0.1:32900:21527168
Call-ID: call-id-0003.000000000000000000server01.example.net
CSeq: 1 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-nelson;box=uk-ed-nelson-04.example-corp.com
To: sip:user.admin@example.org;pool=uk-ed-nelson;box=uk-ed-nelson-04.example-corp.com
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: CallStart
X-slave-site: localhost
X-last-change: 1610755447
X-Conf_no: 705
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.admin@example.org
X-domain: DEFAULT
X-client-address: 10.1.224.108
X-From: user.admin@example.org
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
X-subject: User Eight's Meeting Room
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMTEuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 1026
Content-type: application/sdp

v=0
o=user.admin@example.org 157921484 104 IN IP4 uk-ed-nelson-01.example-corp.com
s=User Eight's Meeting Room
i="Fajar" (+15550000021) CallByPhone-URL
c=TN RFC2543 +15550000021
t=3819744247 0
m=audio 1 voice -
a=status:(2) starting
a=mediastatus:sendrecv
a=leg_no:1
a=flags:1050144
a=new_change
a=server:uk-ed-nelson-04.example-corp.com
a=clir:false:13155500007
a=acs_guid:001028142360
a=cli-screening:00
a=useforfrom:user.admin@example.org
a=legCallid:170540432@198.51.100.74
a=cdr_start_time:1610755447.430349
a=preferred_identity:13155500007
a=x-voice-callowner-login_alias:user.admin@example.org
a=x-voice-legowner-login_alias:user.admin@example.org
a=x-voice-call_source_type:0
a=x-voice-displayname:Fajar
a=x-voice-displayname_type:0
a=x-voice-clientaddress:198.51.100.146:26366
a=x-voice-unique_device_id:00000015-0000-4000-8000-000000000000
a=x-voice-UF1: 
a=ivr:dialout
a=access_code:0000009
a=privs:participant
a=dialout:click_in
a=x-voice-smartproxy:USAZW2-LALTAIR1.example-corp.com


2021-01-16 00:04:17,202643
NOTIFY sip:2-user.admin@example.org;pool=uk-ed-nelson;box=uk-ed-nelson-04.example-corp.com-2 SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__user.admin@example.org;received=127.0.0.1:48114:442357920
Via: SIP/2.0/TCP 127.0.0.1:32900;received=127.0.0.1:32900:21527168
Call-ID: call-id-0003.000000000000000000server01.example.net
CSeq: 3 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-nelson;box=uk-ed-nelson-04.example-corp.com
To: sip:user.admin@example.org;pool=uk-ed-nelson;box=uk-ed-nelson-04.example-corp.com
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: LegAdd
X-slave-site: localhost
X-last-change: 1610755457
X-Conf_no: 705
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.admin@example.org
X-domain: DEFAULT
X-client-address: 10.1.224.108
X-From: user.admin@example.org
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
X-subject: User Eight's Meeting Room
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMTEuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 1150
Content-type: application/sdp

v=0
o=user.admin@example.org 157921484 115 IN IP4 uk-ed-nelson-01.example-corp.com
s=User Eight's Meeting Room
i="Fajar" (+15550000021) CallByPhone-URL
c=TN RFC2543 +15550000021
t=3819744257 0
m=audio 1 voice -
a=status:(203) answered  
a=mediastatus:sendrecv
a=leg_no:1
a=remote:198.51.100.74:12000
a=flags:1050144
a=new_change
a=server:uk-ed-nelson-04.example-corp.com
a=clir:false:13155500007
a=far_end:192.0.2.72:22320
a=acs_guid:001028142360
a=cli-screening:00
a=useforfrom:user.admin@example.org
a=legCallid:170540432@198.51.100.74
a=audio_payload:PCMU
a=cdr_start_time:1610755447.430349
a=preferred_identity:13155500007
a=x-voice-callowner-login_alias:user.admin@example.org
a=x-voice-legowner-login_alias:user.admin@example.org
a=x-voice-call_source_type:0
a=x-voice-displayname:Fajar
a=x-voice-displayname_type:0
a=x-voice-clientaddress:198.51.100.146:26366
a=x-voice-unique_device_id:00000015-0000-4000-8000-000000000000
a=x-voice-UF1: 
a=ivr:dialout
a=access_code:0000009
a=privs:participant
a=dialout:click_in
a=x-voice-smartproxy:USAZW2-LALTAIR1.example-corp.com
a=user-agent:Cisco-SIPGateway/IOS-12.x


2021-01-16 00:04:17,205785
NOTIFY sip:3-user.admin@example.org;pool=uk-ed-nelson;box=uk-ed-nelson-04.example-corp.com-3 SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__user.admin@example.org;received=127.0.0.1:48114:442357920
Via: SIP/2.0/TCP 127.0.0.1:32900;received=127.0.0.1:32900:21527168
Call-ID: call-id-0003.000000000000000000server01.example.net
CSeq: 5 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-nelson;box=uk-ed-nelson-04.example-corp.com
To: sip:user.admin@example.org;pool=uk-ed-nelson;box=uk-ed-nelson-04.example-corp.com
Contact: <sip:localhost:8443;transport=ssl>
X-slave-site: localhost
X-last-change: 1610755457
X-Conf_no: 705
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.admin@example.org
X-domain: DEFAULT
X-client-address: 10.1.224.108
X-From: user.admin@example.org
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
X-subject: User Eight's Meeting Room
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMTEuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 1155
Content-type: application/sdp

v=0
o=user.admin@example.org 157921484 127 IN IP4 uk-ed-nelson-01.example-corp.com
s=User Eight's Meeting Room
i="Fajar" (+15550000021) CallByPhone-URL
c=TN RFC2543 +15550000021
t=3819744257 0
m=audio 1 voice -
a=status:(205) answered hold 
a=mediastatus:nomedia
a=leg_no:1
a=remote:198.51.100.74:12000
a=flags:1050146
a=new_change
a=server:uk-ed-nelson-04.example-corp.com
a=clir:false:13155500007
a=far_end:192.0.2.72:22320
a=acs_guid:001028142360
a=cli-screening:00
a=useforfrom:user.admin@example.org
a=legCallid:170540432@198.51.100.74
a=audio_payload:PCMU
a=cdr_start_time:1610755447.430349
a=preferred_identity:13155500007
a=x-voice-callowner-login_alias:user.admin@example.org
a=x-voice-legowner-login_alias:user.admin@example.org
a=x-voice-call_source_type:0
a=x-voice-displayname:Fajar
a=x-voice-displayname_type:0
a=x-voice-clientaddress:198.51.100.146:26366
a=x-voice-unique_device_id:00000015-0000-4000-8000-000000000000
a=x-voice-UF1: 
a=ivr:dialout
a=access_code:0000009
a=privs:participant
a=dialout:click_in
a=x-voice-smartproxy:USAZW2-LALTAIR1.example-corp.com
a=user-agent:Cisco-SIPGateway/IOS-12.x
