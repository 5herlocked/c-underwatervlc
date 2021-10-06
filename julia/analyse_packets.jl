using CSV, Query, DataFrames, Statistics

function getTransmitterPattern(transmitterVector::Vector{Int16}, precision, ratio)::String
	pattern = ""
	
	for i in range(1, precision; step=1)
		bit = transmitterVector[i]
		for repeat in range(1, ratio; step=1)
			pattern *= string(bit)
		end
	end
	
	return pattern
end

function getReceiverString(receiverVector::Vector{Int16})::String
	receiver = ""
	
	for i in receiverVector
		receiver *= string(i)
	end
	
	return receiver
end

function getVectorFromFile(file::CSV.File)::Vector{Int16}
	fileVector = Vector{Int16}()
	
	for log in file
		push!(fileVector, log.bit)
	end
	
	return fileVector
end

function getBER(sent, received, ratio)::Float64
	success = 0
	rec_counter = 0
	for b in sent
		iSucc = 0
		for i in 1:ratio
			rec_counter += 1
			if received[rec_counter] == b
				iSucc += 1
			end
		end
		
		if iSucc >= ratio/2
			success += 1
		end
	end

	return (1 - success/size(sent)[1])
end

function expandVector(vector, ratio)::Vector
	new_vec = Vector{Int16}()
	
	for i in vector
		for j in range(1, ratio; step=1)
			append!(new_vec, i)
		end
	end
	
	return new_vec
end

begin
    data_location = "E:\\MORSE datasets\\packet_set\\"
    available = readdir(data_location)
    available = available |> @filter(occursin(".csv", _)) |> collect
end

# analyse section
# begin
#     for file_name in available
#         if occursin("transmitter", file_name)
#             continue
#         end

#         # var[1] = transmit rate
#         # var[2] = receive rate
#         # var[3] = ignore - always says packet
#         # var[4] = dataset
#         dataset_specifier = split(split(file_name, ".")[1], "_")

#         ratio = 0

#         if dataset_specifier[1] == "25Hz"
#             ratio = 4
#         elseif dataset_specifier[1] == "50Hz" || dataset_specifier[1] == "15Hz" || dataset_specifier[1] == "30Hz"
#             ratio = 2
#         elseif dataset_specifier[1] == "5Hz"
#             ratio = 3
#         elseif dataset_specifier[1] == "1Hz"
#             ratio = 100
#         end

#         receiver_file = DataFrame(CSV.File(data_location*file_name))
        
#         dF = DataFrame(receiver_file)

#         blue_low = minimum(receiver_file.blue)
#         blue_high = maximum(receiver_file.blue)

#         threshold = blue_low + (blue_high - blue_low)/2
        
#         for row in eachrow(dF)
#             if row.blue < threshold
#                 row.bit = "0"
#             else
#                 row.bit = "1"
#             end
#         end

#         CSV.write(file_name, dF)
#     end
# end

function findPackets(vec, ratio)
    barker_base = parse.(Int, collect("1110010"))
    tail_base = parse.(Int, collect("0000000"))

    compare_barker = expandVector(barker_base, ratio)
    compare_tail = expandVector(tail_base, ratio)

    pilot_auto = Vector{Float64}()
    tail_auto = Vector{Float64}()

    # windowing loop borrowed from https://stackoverflow.com/questions/60886797/sliding-window-function-in-julia
    # for window in view of vec from i to i + (7 bits) * ratio - 1; for i in 1 to length(vec)-(7*ratio)+1 with 1 steps
    for window in ((@view vec[i:i+(7*ratio)-1]) for i in 1:1:length(vec)-(7*ratio)+1)
        barker_correlation = cor(compare_barker, window)
        tail_correlation = cor(compare_tail, window)
        append!(pilot_auto, barker_correlation)
        append!(tail_auto, tail_correlation)
    end

    return (pilot_auto, tail_auto)
end

# ber section
begin
    temp_file = CSV.File(data_location*available[1])
    temp_vec = getVectorFromFile(temp_file)

    pilot_correlation, tail_correlation = findPackets(temp_vec, 4)
end