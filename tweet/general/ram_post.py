consumer_key = 'TynhLBXGxZepWH0WQVf26QdpV'
consumer_secret = 'n7hBsbrQXUTmRgps9pmPYNl85OKy3qyHd15fOZ7W7TD6lM3Rek'
access_token = '1192421889815105536-B6osx8nZSbh6oi7ORt5ydybBZMVpSL'
access_token_secret = 'se2unVv40EJajV3CJMYJnhrFQ43zcJP4ncPX5DtBrQVgI'

api = TwitterAPI(consumer_key,
        consumer_secret,
        access_token,
        access_token_secret)

message=0
message="StartTime:"+str(startdate)
message+= "\nEndTime:" + str(enddate)
message+= "\nMax Value:" + str(high)
message+= "\nMin Value:" + str(low)
message+= "\nAverage:"+str(mean)
message+= "\nVariance"+str(variance)
message+= "\n"

file=open('foo.png','rb')
data=file.read()
r = api.request('statuses/update', {'status': str(message)},{'media[]':data})
print( 'SUCCESS' if r.status_code == 200 else 'FAILURE' )
