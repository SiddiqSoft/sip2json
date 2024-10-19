2020-07-28 05:20:49,586477
NOTIFY sip:masi.zaki@squirepb.com;pool=uk-ed-thames;box=ukdc1-edm13.ring2.com SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__masi.zaki@squirepb.com;received=127.0.0.1:34862:1442942896
Via: SIP/2.0/TCP 127.0.0.1:48868;received=127.0.0.1:48868:26772976
Call-ID: 0702882579444526295.uk-ed-thames-01.ring2.com
CSeq: 116 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-thames;box=ukdc1-edm13.ring2.com
To: sip:masi.zaki@squirepb.com;pool=uk-ed-thames;box=ukdc1-edm13.ring2.com
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
X-control-master: masi.zaki@squirepb.com
X-domain: DEFAULT
X-client-address: 127.0.0.1
X-From: masi.zaki@squirepb.com
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
X-subject: Masi Zaki's Meeting Room
X-project-code: None
X-reserved-ports: 0
X-restrict-participants: false
X-Call-Instance-ID: MDcwMjg4MjU3OTQ0NDUyNjI5NS51ay1lZC10aGFtZXMtMDEucmluZzIuY29tOjE1OTU5MTIyMDY6NzMwMjU2
X-Route-ID: 1
Content-Length: 1872
Content-type: application/sdp

v=0
o=masi.zaki@squirepb.com 57046985175711 847315620 IN IP4 uk-ed-thames-01.ring2.com
s=
i=+44919650054173 (+44919650054173) CallByPhone-URL
c=TN RFC2543 +44919650054173
t=3804900991 3804902449
m=audio 1 voice -
a=status:(4) dropped
a=mediastatus:sendrecv
a=cause: (20001) Normal Hangup - Received SIP BYE From: +44919650054173@10.44.177.35
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
a=cdr_stop_time:1595913649.585872
v=0
o=masi.zaki@squirepb.com 1804662462 847315690 IN IP4 localhost
s=
i=usecallid-leg-4 (usecallid-leg-4) CallByPhone
c=IN IP4 205.252.237.71
t=3804901154 3804902449
m=audio 8766 RTP/AVP 0 101
a=status:(12) ending
a=mediastatus:sendrecv
a=cause: (40027) THS_CAUSE_DROP_CONNECTOR
a=leg_no:4
a=remote:62.189.145.41:12008
a=flags:1049090
a=new_change
a=server:ukdc1-edm13.ring2.com
a=sipphone:usecallid_0702882579444526295.uk-ed-thames-01.ring2.com@205.252.237.71;port=5060
a=clir:false
a=fmtp:101 0-15
a=far_end:205.252.237.71:12126
a=acs_guid:000471698977
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:masi.zaki@squirepb.com
a=legCallid:1909584670@62.189.145.41
a=audio_payload:PCMU
a=cdr_start_time:1595912354.645103
a=rtpmap:0 pcmu/8000/1
a=rtpmap:101 telephone-event/8000
a=dial_once:hk-ed-pearl-01.ring2.com
a=access_code:0000000
a=privs:participant
a=dialout:click_in
a=ivr:dialout
a=user-agent:LoopUp eDial ACS 9.1.0b8071
