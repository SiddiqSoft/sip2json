NOTIFY sip:user.mm@example.com SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/tcp localhost:8443;branch=user.mm@example.com__eDial_sep__user.mm@example.com
Call-ID: callid00240000000000000000000
CSeq: 9 NOTIFY
From: sip:user.mm@example.com;pool=uk-ed-thames;box=ukdc1-edm18.example.net;tag=12345678
To: "mmyers" <sip:user.mm@example.com>
Contact: <sip:localhost:8443;transport=ssl>
X-slave-site: localhost
X-last-change: 1594040299
X-Sticky: 1
X-Conf_no: 236398
X-restrict-participants: false
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.mm@example.com
X-domain: DEFAULT
X-client-address: 10.44.200.95
X-From: user.mm@example.com
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
X-subject: User Two' Meeting Room
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMTUuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
X-Call-Start-Time: 1594040277.665005
Content-Length: 892
Content-type: application/sdp

v=0
o=user.mm@example.com 1811084562 854064065 IN IP4 localhost
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
a=server:ukdc1-edm18.example.net
a=sipphone:usecallid_callid00240000000000000000000@10.254.254.33;port=5060
a=clir:false
a=fmtp:101 0-15
a=far_end:10.254.254.33:12196
a=acs_guid:001712266534
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.mm@example.com
a=legCallid:1445106199@203.0.113.107
a=audio_payload:PCMU
a=rtpmap:0 pcmu/8000/1
a=rtpmap:101 telephone-event/8000
a=dial_once:ukdc1-edm13.example.net
a=access_code:0000000
a=privs:participant
a=dialout:click_in
a=ivr:dialout
a=user-agent:Example eDial ACS 9.1.0b8050
