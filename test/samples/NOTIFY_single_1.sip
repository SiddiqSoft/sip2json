NOTIFY sip:masi.zaki@squirepb.com;pool=uk-ed-thames;box=ukdc1-edm13.ring2.com SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__masi.zaki@squirepb.com;received=127.0.0.1:34862:1442942896
Via: SIP/2.0/TCP 127.0.0.1:48868;received=127.0.0.1:48868:26772976
Call-ID: 0702882579444526295.uk-ed-thames-01.ring2.com
CSeq: 51 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-thames;box=ukdc1-edm13.ring2.com
To: sip:masi.zaki@squirepb.com;pool=uk-ed-thames;box=ukdc1-edm13.ring2.com
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
X-control-master: masi.zaki@squirepb.com
X-domain: DEFAULT
X-client-address: 127.0.0.1
X-From: masi.zaki@squirepb.com
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
X-subject: Masi Zaki's Meeting Room
X-project-code: None
X-reserved-ports: 0
X-restrict-participants: false
X-active-talker:
X-Call-Instance-ID: MDcwMjg4MjU3OTQ0NDUyNjI5NS51ay1lZC10aGFtZXMtMDEucmluZzIuY29tOjE1OTU5MTIyMDY6NzMwMjU2
X-Route-ID: 1
Content-Length: 1716
Content-type: application/sdp

v=0
o=masi.zaki@squirepb.com 57046985175711 847315547 IN IP4 uk-ed-thames-01.ring2.com
s=
i=+44919650054173 (+44919650054173) CallByPhone-URL
c=TN RFC2543 +44919650054173
t=3804900991 0
m=audio 1 voice -
a=status:(203) answered
a=mediastatus:sendrecv
a=leg_no:1
a=remote:62.189.145.41:12002
a=flags:1049634
a=new_change
a=server:ukdc1-edm13.ring2.com
a=clir:false
a=far_end:10.44.177.35:38880
a=acs_guid:000471698977
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:+44044987799@62.189.145.41
a=audio_payload:PCMU
a=cdr_start_time:1595912191.862391
a=access_code:13773497
a=privs:participant
a=ivr:dialin
a=creator:masi.zaki@squirepb.com
a=dialin:13773497:6660013449@10.44.177.35-$$-1
a=dest_phone:+44044987799
a=user-agent:Cisco-SIPGateway/IOS-16.9.4
v=0
o=masi.zaki@squirepb.com 874381890 874381994 IN IP4 uk-ed-thames-01.ring2.com
s=0702882579444526295.uk-ed-thames-01.ring2.com
i=Recording by masi.zaki@squirepb.com (0702882579444526295.uk-ed-thames-01.ring2.com) CallByPhone-URL
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
a=server:ukdc1-edm13.ring2.com
a=clir:false
a=fmtp:x-record uri:file:4D4463774D6A67344D6A55334F5451304E4455794E6A49354E5335316179316C5A433130614746745A584D744D444575636D6C755A7A4975593239744F6A45314F5455354D5449794D4459364E7A4D774D6A5532/recording.wav;host=10.254.254.1;access_code=13773497
a=machine_leg:1
a=acs_guid:000471698977
a=x-ring2-call_source_type:0
a=x-ring2-smartproxy:usdc2-asalt04.ring2.com
a=x-ring2-unique_device_id:3f62905d-97b3-4743-8fe4-7f492d577761
a=x-ring2-clientaddress:121.209.66.56:54290

