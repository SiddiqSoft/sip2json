NOTIFY sip:user.lab.voip@example.org;pool=uk-ed-nelson;box=uk-ed-nelson-04.example-corp.com SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__user.lab.voip@example.org;received=127.0.0.1:48114:442357920
Via: SIP/2.0/TCP 127.0.0.1:32900;branch=something@somewhere.host;received=127.0.0.1:32900:215271
Via: SIP/2.0/TCP 127.0.0.1:32900;received=127.0.0.1:32900:21527168
Call-ID: call-id-0006.00000000000000000000server01.example.net
CSeq: 1 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-nelson;box=uk-ed-nelson-04.example-corp.com
To: sip:user.lab.voip@example.org;pool=uk-ed-nelson;box=uk-ed-nelson-04.example-corp.com
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: CallStart
X-slave-site: localhost
X-last-change: 1610824775
X-Conf_no: 711
X-rollcall: disabled
X-Call-URL: true
X-control-master: user.lab.voip@example.org
X-domain: DEFAULT
X-client-address: 10.1.224.108
X-From: user.lab.voip@example.org
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
X-leader-required: true
X-Video-UsingMCU: false
X-subject: Voip User's Meeting Room
X-project-code: df
X-Call-Instance-ID: YW5vbnltaXplZC1jYWxsLWluc3RhbmNlLWlkLTAwMTYuZXhhbXBsZS5uZXQ6MTYwMDAwMDAwMDowMDAwMDA=
X-Route-ID: 1
Content-Length: 1031
Content-type: application/sdp

v=0
o=user.lab.voip@example.org 227247312 104 IN IP4 uk-ed-nelson-01.example-corp.com
s=Voip User's Meeting Room
i="Voip%20User" (+15550000019) CallByPhone-URL
c=TN RFC2543 +15550000019
t=3819813575 0
m=audio 1 voice -
a=status:(2) starting
a=mediastatus:sendrecv
a=leg_no:1
a=flags:1054240
a=new_change
a=server:uk-ed-nelson-04.example-corp.com
a=clir:false:13155500007
a=acs_guid:000006832408
a=cli-screening:00
a=useforfrom:user.lab.voip@example.org
a=legCallid:170540458@198.51.100.74
a=cdr_start_time:1610824775.753977
a=preferred_identity:13155500007
a=x-ring2-callowner-login_alias:user.lab.voip@example.org
a=x-ring2-legowner-login_alias:user.lab.voip@example.org
a=x-ring2-alt-proj:df
a=x-ring2-call_source_type:0
a=x-ring2-displayname:Voip%20User
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:192.0.2.115:48852
a=x-ring2-unique_device_id:00000017-0000-4000-8000-000000000000
a=x-ring2-UF1:df
a=ivr:dialout
a=dialout:click_in
a=access_code:0000004
a=privs:leader
a=x-ring2-smartproxy:USAZW2-LALTAIR1.example-corp.com
