--http://randomnerdtutorials.com/esp8266-web-server/ 
wifi.setmode(wifi.STATION)
wifi.sta.config("AndroidAP2","abcd1234")
-- wifi.sta.config("iptime","abcd1234")
print(wifi.sta.getip())
led1 = 1
led2 = 2
led3 = 3
led4 = 5
i = 0
gpio.mode(led1, gpio.OUTPUT)
gpio.mode(led2, gpio.OUTPUT)
gpio.mode(led3, gpio.OUTPUT)
gpio.mode(led4, gpio.OUTPUT)
srv=net.createServer(net.TCP)


function blink(led, timer)
   if i % 2 == 0 then
      gpio.write(led, gpio.LOW)
   else 
      gpio.write(led, gpio.HIGH)
   end
   i=i+1
   if (i == 6) then
      tmr.stop(timer)
      i = 0
   end
end

function blink1()
    blink(led1, 0);
end
function blink2()
    blink(led2, 1);
end
function blink3()
    blink(led3, 2);
end
function blink4()
    blink(led4, 3);
end


srv:listen(80,function(conn)
    conn:on("receive", function(client,request)
        local buf = "";
        local _, _, method, path, vars = string.find(request, "([A-Z]+) (.+)?(.+) HTTP");
        if(method == nil)then
            _, _, method, path = string.find(request, "([A-Z]+) (.+) HTTP");
        end
        local _GET = {}
        if (vars ~= nil)then
            for k, v in string.gmatch(vars, "(%w+)=(%w+)&*") do
                _GET[k] = v
            end
        end
        buf = buf.."<h1>Loverator</h1>";
        buf = buf.."<p>N1 (D1)";
        buf = buf.."&nbsp;<a href=\"?node=1w\"><button>week</button></a>";
        buf = buf.."&nbsp;<a href=\"?node=1s\"><button>strong</button></a></p>";

        buf = buf.."<p>N2 (D2)";
        buf = buf.."&nbsp;<a href=\"?node=2w\"><button>week</button></a>";
        buf = buf.."&nbsp;<a href=\"?node=2s\"><button>strong</button></a></p>";

        buf = buf.."<p>N3 (D3)";
        buf = buf.."&nbsp;<a href=\"?node=3w\"><button>week</button></a>";
        buf = buf.."&nbsp;<a href=\"?node=3s\"><button>strong</button></a></p>";

        buf = buf.."<p>N4 (D4)";
        buf = buf.."&nbsp;<a href=\"?node=4w\"><button>week</button></a>";
        buf = buf.."&nbsp;<a href=\"?node=4s\"><button>strong</button></a></p>";

        local _on,_off = "",""
       if(_GET.node == "1w")then
             tmr.alarm(0, 300, 1, blink1);
       elseif(_GET.node == "1s")then
             tmr.alarm(0, 30, 3, blink1);
       elseif(_GET.node == "2w")then
             tmr.alarm(1, 300, 1, blink2);
       elseif(_GET.node == "2s")then
             tmr.alarm(1, 30, 3, blink2);
       elseif(_GET.node == "3w")then
             tmr.alarm(2, 300, 1, blink3);
       elseif(_GET.node == "3s")then
             tmr.alarm(2, 30, 3, blink3);
       elseif(_GET.node == "4w")then
             tmr.alarm(3, 300, 1, blink4);
       elseif(_GET.node == "4s")then
             tmr.alarm(3, 30, 3, blink4);
        end
        client:send(buf);
        client:close();
        collectgarbage();
    end)
end)
