gpick.serialize_normal = function(color_object)
	local c = color_object:get_color()
	return string.format('n: (%.1f,%.1f,%.1f)', (c:red() * 2.0) - 1.0, (c:green() * 2.0) - 1.0, (c:blue() * 2.0) - 1.0)

end

gpick.converters['normals'] = {
	human_readable = 'normals',
	serialize = gpick.serialize_normal,
	deserialize = nil 
};
