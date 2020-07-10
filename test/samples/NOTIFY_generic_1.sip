NOTIFY sip:hello@world.com SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/tcp localhost:8443;branch=hello@world.com__eDial_sep__hello@world.com
Call-ID: 80000000000000aa-aa-aaaaaa-00
CSeq: 9 NOTIFY
From: sip:hello@world.com;pool=uk-ed-thames;box=ukdc1-edm18.ring2.com;tag=12345678
To: "mmyers" <sip:hello@world.com>
Contact: <sip:localhost:8443;transport=ssl>
X-slave-site: localhost
X-last-change: 1594040299
X-Sticky: 1
X-Conf_no: 236398
X-restrict-participants: false
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: hello@world.com
X-domain: DEFAULT
X-client-address: 10.44.200.95
X-From: hello@world.com
X-legs-on-server: 244
X-rss-id: 
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
X-subject: Robin Myers' Meeting Room
X-Call-Instance-ID: ODQ0NDMaNaU5MaaaOTa1aa1lZC10aGFtZXMtMDE6MTU5NDA0MDI3Naa2NjQ5Nja=
X-Route-ID: 1
X-Call-Start-Time: 1594040277.665005
Content-Length: 880
Content-type: application/sdp

v=0
o=hello@world.com 1011084562 804064065 IN IP4 localhost
s=
i=usecallid-leg-3 (usecallid-leg-3) CallByPhone
c=IN IP4 10.254.254.33
t=3803029099 0
m=audio 8766 RTP/AVP 0 101
a=status:(205) answered hold 
a=mediastatus:nomedia
a=cdr_start_time:1594555399.0
a=leg_no:3
a=remote:10.254.254.38:12224
a=flags:1049122
a=new_change
a=server:ukdc1-edm18.ring2.com
a=sipphone:usecallid_80000000000000aa-aa-aaaaaa-00@10.254.254.33;port=5060
a=clir:false
a=fmtp:101 0-15
a=far_end:10.254.254.33:12196
a=acs_guid:001010000004
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:hello@world.com
a=legCallid:1000000009@10.100.100.100
a=audio_payload:PCMU
a=rtpmap:0 pcmu/8000/1
a=rtpmap:101 telephone-event/8000
a=dial_once:aaaa1-aaa13.aaaa2.com
a=access_code:0000000
a=privs:participant
a=dialout:click_in
a=ivr:dialout
a=user-agent:LoopUp eDial ACS 9.1.0b8050
