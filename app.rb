#Sinatra and Websocket Server
require 'thin'

require 'em-websocket'
require 'sinatra/base'


#sinatra server
EM.run do
    class App < Sinatra::Base
        get '/' do
            erb :index
        end
        set :bind, '0.0.0.0'
    end

    @clients = []
#event-machine websocket server
    EM::WebSocket.start(:host => '0.0.0.0', :port => '3001') do |ws|
        ws.onopen do |handshake|
            @clients << ws
            ws.send "Connected to #{handshake.path}."
            puts "Connected"
        end

        ws.onclose do
            ws.send "Closed."
            puts "closed"
            @clients.delete ws
        end

        ws.onmessage do |msg|
            puts "Received Message: #{msg}"
	#message received from the internal channel of the server
            @clients.each do |socket|
                socket.send msg
	#the message getting received is being sent to all other connect clients
            end
        end
    end

    App.run! :port => 3000
#browser client can be connected to port 3000
end
