
#machine client ruby
require_relative 'em-websocket-client'
require 'serialport'
#SerialPort library in ruby




EM.run do
    conn = EventMachine::WebSocketClient.connect("ws://0.0.0.0:3001/")

    conn.callback do
        conn.send_msg "connected!"
    end

    conn.errback do |e|
        puts "Got error: #{e}"
    end

    conn.stream do |msg|
	#message getting received from the server internal channel (which is being received from the browser client)
        puts msg
        port_file = "/dev/ttyACM0" #portfile address (where the tm4c123g is connected from the raspberry pi)
        baud_rate = 115200         #The same baudrate that is set for the TIVA board
	data_bits = 8              #8 data bits
	stop_bits = 1		   #1 stop bit
	parity = SerialPort::NONE  #no parity bit
	ser = SerialPort.new(port_file , baud_rate , data_bits , stop_bits , parity)
	#opening the serialport connection through the ser object
		
	ser.write("#{msg}") 
	#write the same message to the serialport from the raspberry pi 
    end

    conn.disconnect do
        puts "disconnected!"
        EM::stop_event_loop
    end
end
