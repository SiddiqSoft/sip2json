NOTIFY sip:lab.edial.rc.116.voip@loopup.co;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com SIP/2.0
Via: SIP/2.0/TCP localhost:780;branch=conference.wizard@local.host__eDial_sep__lab.edial.rc.116.voip@loopup.co;received=127.0.0.1:48114:442357920
Via: SIP/2.0/TCP 127.0.0.1:32900;branch=something@somewhere.host;received=127.0.0.1:32900:215271
Via: SIP/2.0/TCP 127.0.0.1:32900;received=127.0.0.1:32900:21527168
Call-ID: 9355232592588793289872.uk-ed-nelson-01.ring2-corp.com
CSeq: 1 NOTIFY
From: sip:conference.wizard@local.host;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com
To: sip:lab.edial.rc.116.voip@loopup.co;pool=uk-ed-nelson;box=uk-ed-nelson-04.ring2-corp.com
Contact: <sip:localhost:8443;transport=ssl>
X-CallEvent: CallStart
X-slave-site: localhost
X-last-change: 1610824775
X-Conf_no: 711
X-rollcall: disabled
X-Call-URL: true
X-control-master: lab.edial.rc.116.voip@loopup.co
X-domain: DEFAULT
X-client-address: 10.1.224.108
X-From: lab.edial.rc.116.voip@loopup.co
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
X-Call-Instance-ID: OTM1NTIzMjU5MjU4ODc5MzI4OTg3Mi51ay1lZC1uZWxzb24tMDEucmluZzItY29ycC5jb206MTYxMDgyNDc3NTo1NTcyMjc=
X-Route-ID: 1
Content-Length: 1055
Content-type: application/sdp

v=0
o=lab.edial.rc.116.voip@loopup.co 227247312 104 IN IP4 uk-ed-nelson-01.ring2-corp.com
s=Voip User's Meeting Room
i="Voip%20User" (+19172084495) CallByPhone-URL
c=TN RFC2543 +19172084495
t=3819813575 0
m=audio 1 voice -
a=status:(2) starting
a=mediastatus:sendrecv
a=leg_no:1
a=flags:1054240
a=new_change
a=server:uk-ed-nelson-04.ring2-corp.com
a=clir:false:13127560923
a=acs_guid:000006832408
a=cli-screening:00
a=useforfrom:lab.edial.rc.116.voip@loopup.co
a=legCallid:170540458@67.131.238.74
a=cdr_start_time:1610824775.753977
a=preferred_identity:13127560923
a=x-ring2-callowner-login_alias:lab.edial.rc.116.voip@loopup.co
a=x-ring2-legowner-login_alias:lab.edial.rc.116.voip@loopup.co
a=x-ring2-alt-proj:df
a=x-ring2-call_source_type:0
a=x-ring2-displayname:Voip%20User
a=x-ring2-displayname_type:0
a=x-ring2-clientaddress:135.180.107.115:48852
a=x-ring2-unique_device_id:d40835f7-2f7f-451f-9920-8afcb0f09a95
a=x-ring2-UF1:df
a=ivr:dialout
a=dialout:click_in
a=access_code:1218771
a=privs:leader
a=x-ring2-smartproxy:USAZW2-LALTAIR1.ring2-corp.com
