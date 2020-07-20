NOTIFY sip:subscribe_to_call_events@loopup.com;machine SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/UDP localhost:780;branch=denver.peterson@loopup.com__eDial_sep__subscribe_to_call_events@loopup.com;received=localhost
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__denver.peterson@loopup.com;received=127.0.0.1:48196:1743178256
Via: SIP/2.0/TCP 127.0.0.1:40150;received=127.0.0.1:40150:23823872
Call-ID: 777447022768721366.il-ed-mara-01.ring2.com
CSeq: 1 NOTIFY
From: sip:conference.wizard@local.host;pool=il-ed-mara;box=il-ed-mara-06.ring2.com
To: "denver.peterson" <sip:denver.peterson@loopup.com>
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: CallStart
X-slave-site: localhost
X-last-change: 1595052854
X-Conf_no: 64606
X-rollcall: disabled
X-Call-URL: true
X-control-master: denver.peterson@loopup.com
X-domain: DEFAULT
X-client-address: 10.1.214.95
X-From: denver.peterson@loopup.com
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
X-Call-Instance-ID: Nzc3NDQ3MDIyNzY4NzIxMzY2LmlsLWVkLW1hcmEtMDEucmluZzIuY29tOjE1OTUwNTI4NTQ6Mjk4NDQy
X-Route-ID: 2
Content-Length: 1081
Content-type: application/sdp

v=0
o=denver.peterson@loopup.com 21303359 103 IN IP4 il-ed-mara-01.ring2.com
s=Altair & Friends Scrum Meeting
i="Ring2QA%20Cingular" (+19253230928) CallByPhone-URL
c=TN RFC2543 +19253230928
t=3804041654 0
m=audio 1 voice -
a=status:(2) starting
a=mediastatus:sendrecv
a=leg_no:1
a=flags:1181232
a=new_change
a=primary:true
a=server:il-ed-mara-06.ring2.com
a=clir:false:18777464263
a=acs_guid:001389554202
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:denver.peterson@loopup.com
a=legCallid:1906635862@216.111.92.36
a=cdr_start_time:1595052854.503881
a=preferred_identity:18777464263
a=x-ring2-callowner-login_alias:denver.peterson@loopup.com
a=x-ring2-legowner-login_alias:denver.peterson@loopup.com
a=x-ring2-call_source_type:0
a=x-ring2-displayname:Ring2QA%20Cingular
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:76.14.53.154:19411
a=x-ring2-unique_device_id:c8169c5a-d1db-412c-9957-5cf33f3bd7a9
a=x-ring2-UF1: 
a=ivr:dialout
a=access_code:2719318
a=privs:participant
a=dialout:click_in
a=x-ring2-smartproxy:usaznc-asalt05.ring2.com
