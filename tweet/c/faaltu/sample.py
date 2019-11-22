import tweepy
#from TwitterAPI import TwitterAPI

consumer_key = 'TynhLBXGxZepWH0WQVf26QdpV'
consumer_secret = 'n7hBsbrQXUTmRgps9pmPYNl85OKy3qyHd15fOZ7W7TD6lM3Rek'
access_token = '1192421889815105536-B6osx8nZSbh6oi7ORt5ydybBZMVpSL'
access_token_secret = 'se2unVv40EJajV3CJMYJnhrFQ43zcJP4ncPX5DtBrQVgI'

def OAuth():
    try:
        auth = tweepy.OAuthHandler(consumer_key,consumer_secret)
        auth.set_access_token(access_token,access_token_secret)
        return auth
    except:
        return None

oauth = OAuth()
api = tweepy.API(oauth)
api.update_status('Sample tweet using python')
print('A tweet was posted')
