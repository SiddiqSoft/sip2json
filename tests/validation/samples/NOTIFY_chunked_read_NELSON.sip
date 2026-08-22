NOTIFY sip:subscribe_to_call_events@example.com;machine SIP/2.0
Via: SIP/2.0/tcp localhost:8443
Via: SIP/2.0/TCP localhost:780;branch=user.tn454@example.org__eDial_sep__subscribe_to_call_events@example.com;received=127.0.0.1:54832:426076176
Via: SIP/2.0/TCP localhost:780;branch=user.tn454@example.org__eDial_sep__user.tn454@example.org;received=127.0.0.1:57610:426417472
Call-ID: callid002400000000000000000000
CSeq: 10 NOTIFY
From: sip:user.tn454@example.org;pool=uk-ed-nelson;box=uk-ed-nelson-01.example-corp.com
To: "testnelson454" <sip:user.tn454@example.org>
Contact: sip:localhost:8443;pool=uk-ed-nelson;box=uk-ed-nelson-02.example-corp.com
X-CallEvent: CallEnd
X-max-leg-no: 2
X-remote-leader: true
X-slave-site: localhost
X-last-change: 1597954107
X-Conf_no: 1211
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.tn454@example.org
X-domain: DEFAULT
X-client-address: 10.1.224.11
X-From: user.tn454@example.org
X-legs-on-server: 24
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
X-subject: User 454's Meeting Room
X-project-code: AlotOfCalls_1::1
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMDYuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 1301
Content-type: application/sdp

v=0
o=user.tn454@example.org 4926060015 238 IN IP4 uk-ed-nelson-01.example-corp.com
s=User 454's Meeting Room
i="Leg--2" (+15550000015) CallByPhone-URL
c=TN RFC2543 +15550000015
t=3806942908 3806942919
m=audio 1 voice -
a=status:(4) dropped
a=mediastatus:sendrecv
a=cause: (30009) dropped by TNS
a=leg_no:2
a=remote:198.51.100.82:12052
a=flags:1054242
a=new_change
a=server:uk-ed-nelson-05.example-corp.com
a=clir:false:16655500009
a=far_end:192.0.2.72:16734
a=acs_guid:002064708633
a=cli-screening:00
a=trunk:8:chan:0
a=useforfrom:user.tn454@example.org
a=legCallid:911795118@198.51.100.82
a=audio_payload:PCMU
a=local_mos:42
a=cdr_start_time:1597954108.764211
a=preferred_identity:16655500009
a=x-ring2-callowner-login_alias:user.tn454@example.org
a=x-ring2-legowner-login_alias:user.tn454@example.org
a=x-ring2-alt-proj:AlotOfCalls_1::1
a=x-ring2-call_source_type:0
a=x-ring2-displayname:Leg--2
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:192.0.2.46:38906
a=x-ring2-unique_device_id:0000000b-0000-4000-8000-000000000000
a=x-ring2-UF1:AlotOfCalls_1::1
a=ivr:dialout
a=dialout:click_in
a=access_code:0000002
a=privs:leader
a=x-ring2-smartproxy:usazw-eassp301.example-corp.com
a=user-agent:Cisco-SIPGateway/IOS-12.x
a=cdr_stop_time:1597954119.914608
