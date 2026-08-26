INVITE sip:service@10.0.0.1:5060 SIP/2.0
Via: SIP/2.0/UDP 10.0.0.2:5060;branch=z9hG4bK-sipp-uac-1
From: sipp <sip:sipp@10.0.0.2:5060>;tag=1
To: service <sip:service@10.0.0.1:5060>
Call-ID: 1-1000@10.0.0.2
CSeq: 1 INVITE
Contact: sip:sipp@10.0.0.2:5060
Max-Forwards: 70
Subject: SIPp UAC Test Scenario
Content-Type: application/sdp
Content-Length: 118

v=0
o=user1 53655765 23536879 IN IP4 10.0.0.2
s=-
c=IN IP4 10.0.0.2
t=0 0
m=audio 6000 RTP/AVP 0
a=rtpmap:0 PCMU/8000
