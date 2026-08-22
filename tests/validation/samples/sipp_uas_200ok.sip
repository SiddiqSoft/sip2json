SIP/2.0 200 OK
Via: SIP/2.0/UDP 10.0.0.2:5060;branch=z9hG4bK-sipp-uac-1;received=10.0.0.2
From: sipp <sip:sipp@10.0.0.2:5060>;tag=1
To: service <sip:service@10.0.0.1:5060>;tag=2
Call-ID: 1-1000@10.0.0.2
CSeq: 1 INVITE
Contact: <sip:service@10.0.0.1:5060;transport=UDP>
Content-Type: application/sdp
Content-Length: 118

v=0
o=user2 53655766 23536880 IN IP4 10.0.0.1
s=-
c=IN IP4 10.0.0.1
t=0 0
m=audio 6002 RTP/AVP 0
a=rtpmap:0 PCMU/8000
