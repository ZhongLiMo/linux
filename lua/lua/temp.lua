Temp = {}

function Temp:dayin(numl)
	print(numl)
end

function Temp:jiafa(numl, num2)
	log(numl..'+'..num2..'='..(numl + num2))
	--log(debug.traceback())
	return (numl + num2)
end


print("5-6="..(lcc:jianfa(5, 6)))