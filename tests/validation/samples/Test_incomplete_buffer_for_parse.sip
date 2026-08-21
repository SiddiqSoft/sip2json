NOTIFY sip:subscribe_to_call_events@example.com;machine SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/UDP localhost:780;branch=user.ak@example.com__eDial_sep__subscribe_to_call_events@example.com;received=localhost
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__user.ak@example.com;received=127.0.0.1:57400:1752919488
Via: SIP/2.0/TCP 127.0.0.1:46942;received=127.0.0.1:46942:36406400
Call-ID: callid001200000000000000000
CSeq: 2 NOTIFY
From: sip:conference.wizard@local.host;pool=hk-ed-pearl;box=hk-ed-pearl-01.example.net
To: "akirmayer" <sip:user.ak@example.com>
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: LegAdd
X-slave-site: localhost
X-last-change: 1593567745
X-drop-with-leader: false
X-Conf_no: 22977
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.ak@example.com
X-domain: DEFAULT
X-client-address: 127.0.0.1
X-From: user.ak@example.com
X-legs-on-server: 138
X-Sticky: 1
X-rss-id: 