NOTIFY sip:mmyers@qinetiq.com SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/tcp localhost:8443;branch=mmyers@qinetiq.com__eDial_sep__mmyers@qinetiq.com
Call-ID: 84443275927297uk-ed-thames-01
CSeq: 9 NOTIFY
From: sip:mmyers@qinetiq.com;pool=uk-ed-thames;box=ukdc1-edm18.ring2.com;tag=12345678
To: "mmyers" <sip:mmyers@qinetiq.com>
Contact: <sip:localhost:8443;transport=ssl>
X-slave-site: localhost
X-last-change: 1594040299
X-Sticky: 1
X-Conf_no: 236398
X-restrict-participants: false
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: mmyers@qinetiq.com
X-domain: DEFAULT
X-client-address: 10.44.200.95
X-From: mmyers@qinetiq.com
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
X-Call-Instance-ID: ODQ0NDMyNzU5MjcyOTd1ay1lZC10aGFtZXMtMDE6MTU5NDA0MDI3Nzo2NjQ5Njg=
X-Route-ID: 1
X-Call-Start-Time: 1594040277.665005
Content-Length: 886
Content-type: application/sdp

v=0
o=mmyers@qinetiq.com 1811084562 854064065 IN IP4 localhost
s=
i=usecallid-leg-3 (usecallid-leg-3) CallByPhone
c=IN IP4 10.254.254.33
t=3803029099 0
m=audio 8766 RTP/AVP 0 101
a=status:(205) answered hold 
a=mediastatus:nomedia
a=cdr_start_time:1594040299.0
a=leg_no:3
a=remote:10.254.254.38:12224
a=flags:1049122
a=new_change
a=server:ukdc1-edm18.ring2.com
a=sipphone:usecallid_84443275927297uk-ed-thames-01@10.254.254.33;port=5060
a=clir:false
a=fmtp:101 0-15
a=far_end:10.254.254.33:12196
a=acs_guid:001712266534
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:mmyers@qinetiq.com
a=legCallid:1445106199@62.189.140.107
a=audio_payload:PCMU
a=rtpmap:0 pcmu/8000/1
a=rtpmap:101 telephone-event/8000
a=dial_once:ukdc1-edm13.ring2.com
a=access_code:0000000
a=privs:participant
a=dialout:click_in
a=ivr:dialout
a=user-agent:LoopUp eDial ACS 9.1.0b8050
