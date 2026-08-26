NOTIFY sip:subscribe_to_call_events@example.com;machine SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/TCP localhost:780;branch=user.mg@example.com__eDial_sep__subscribe_to_call_events@example.com;received=127.0.0.1:44350:1752919488
Via: SIP/2.0/TCP localhost:780;branch=user.mg@example.com__eDial_sep__user.mg@example.com;received=127.0.0.1:48606:20570384
Call-ID: callid0020000000000000000000
CSeq: 49 NOTIFY
From: sip:user.mg@example.com;pool=il-ed-mara;box=il-ed-mara-01.example.net
To: "matthew.gabbard" <sip:user.mg@example.com>
Contact: sip:localhost:8443;pool=il-ed-mara;box=il-ed-mara-02.example.net
X-max-leg-no: 5
X-CallEvent: CallEnd
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1593637170
X-Sticky: 1
X-Conf_no: 32667
X-restrict-participants: false
X-no-unmute: false
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.mg@example.com
X-domain: DEFAULT
X-client-address: 10.1.206.91
X-From: user.mg@example.com
X-legs-on-server: 286
X-rss-id: 
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
X-subject: User Three's Meeting Room
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDQuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 2
X-Call-Start-Time: 1593637170.156760
Content-Length: 1291
Content-type: application/sdp

v=0
o=user.mg@example.com 3598380125 241 IN IP4 il-ed-mara-01.example.net
s=User Three's Meeting Room
i="User%20Three%20-%20" (+15550000014) CallByPhone-URL
c=T-NRFC/2543+15550000014(hi)
t=3802625984 3802626770
m=audio 1 voice -
a=status:(4) dropped
a=mediastatus:sendrecv
a=cdr_start_time:1593637184.0
a=cause: (20001) Normal Hangup - Received SIP BYE From: 12124553521@192.0.2.73
a=leg_no:2
a=remote:192.0.2.36:12298
a=flags:1054242
a=new_change
a=server:il-ed-mara-06.example.net
a=clir:false:18755500010
a=far_end:192.0.2.73:33576
a=acs_guid:000414323482
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.mg@example.com
a=legCallid:646390624@192.0.2.36
a=audio_payload:PCMU
a=preferred_identity:18755500010
a=x-voice-callowner-login_alias:user.mg@example.com
a=x-voice-legowner-login_alias:user.mg@example.com
a=x-voice-alt-proj: 
a=x-voice-call_source_type:16
a=x-voice-displayname:User%20Three%20-%20
a=x-voice-displayname_type:1
a=x-voice-clientaddress:198.51.100.187:47553
a=x-voice-unique_device_id:0000000000000000000000000000000000000000000000000000000000000001
a=x-voice-UF1: 
a=ivr:dialout
a=dialout:click_in
a=access_code:0000018
a=privs:leader
a=x-voice-smartproxy:usaze-asalt01.example.net
a=user-agent:Cisco-SIPGateway/IOS-16.3.3
