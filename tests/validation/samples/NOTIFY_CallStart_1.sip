NOTIFY sip:subscribe_to_call_events@example.com;machine SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/UDP localhost:780;branch=denver.peterson@example.com__eDial_sep__subscribe_to_call_events@example.com;received=localhost
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__denver.peterson@example.com;received=127.0.0.1:48196:1743178256
Via: SIP/2.0/TCP 127.0.0.1:40150;received=127.0.0.1:40150:23823872
Call-ID: call-id-0005.000000000server01.example.net
CSeq: 1 NOTIFY
From: sip:conference.wizard@local.host;pool=il-ed-mara;box=il-ed-mara-06.example.net
To: "denver.peterson" <sip:denver.peterson@example.com>
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: CallStart
X-slave-site: localhost
X-last-change: 1595052854
X-Conf_no: 64606
X-rollcall: disabled
X-Call-URL: true
X-control-master: denver.peterson@example.com
X-domain: DEFAULT
X-client-address: 10.1.214.95
X-From: denver.peterson@example.com
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
X-Video-UsingMCU: false
X-subject: Altair & Friends Scrum Meeting
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMTMuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 2
Content-Length: 1088
Content-type: application/sdp

v=0
o=denver.peterson@example.com 21303359 103 IN IP4 il-ed-mara-01.example.net
s=Altair & Friends Scrum Meeting
i="SampleQA%20Cingular" (+15550000020) CallByPhone-URL
c=TN RFC2543 +15550000020
t=3804041654 0
m=audio 1 voice -
a=status:(2) starting
a=mediastatus:sendrecv
a=leg_no:1
a=flags:1181232
a=new_change
a=primary:true
a=server:il-ed-mara-06.example.net
a=clir:false:18755500010
a=acs_guid:001389554202
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:denver.peterson@example.com
a=legCallid:1906635862@192.0.2.36
a=cdr_start_time:1595052854.503881
a=preferred_identity:18755500010
a=x-ring2-callowner-login_alias:denver.peterson@example.com
a=x-ring2-legowner-login_alias:denver.peterson@example.com
a=x-ring2-call_source_type:0
a=x-ring2-displayname:SampleQA%20Cingular
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:192.0.2.13:19411
a=x-ring2-unique_device_id:00000013-0000-4000-8000-000000000000
a=x-ring2-UF1: 
a=ivr:dialout
a=access_code:0000013
a=privs:participant
a=dialout:click_in
a=x-ring2-smartproxy:usaznc-asalt05.example.net
