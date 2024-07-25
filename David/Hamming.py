def calculate_parity_bits(hamming_code):
    n = len(hamming_code)
    r = 0

    # Calculate number of parity bits
    while (2**r < n + 1):
        r += 1

    parity_bits = []
    for i in range(r):
        parity_pos = 2 ** i
        parity = 0
        for j in range(1, n + 1):
            if j & parity_pos:
                parity ^= hamming_code[j - 1]
        parity_bits.append(parity)

    return parity_bits

def check_and_correct_hamming_code(hamming_code):
    n = len(hamming_code)
    r = 0

    # Calculate number of parity bits
    while (2**r < n + 1):
        r += 1

    # Calculate parity bits and find the error position
    error_pos = 0
    for i in range(r):
        parity_pos = 2 ** i
        parity = 0
        for j in range(1, n + 1):
            if j & parity_pos:
                parity ^= hamming_code[j - 1]
        if parity != 0:
            error_pos += parity_pos

    if error_pos:
        print(f"Error detected at bit position: {error_pos}")
        # Correct the error
        hamming_code[error_pos - 1] ^= 1
        print("Error corrected.")
        
        # Recalculate parity bits to check for multiple errors
        recalculated_parity_bits = calculate_parity_bits(hamming_code)
        if any(recalculated_parity_bits):
            print("Multiple errors detected. Message discarded.")
            return [], error_pos, True

    # Extract the original message
    original_message = []
    for i in range(1, n + 1):
        if not (i & (i - 1)) == 0:
            original_message.append(hamming_code[i - 1])

    return original_message, error_pos, False

def main():
    input_code = input("Enter the received Hamming code: ")
    hamming_code = [int(bit) for bit in input_code]

    original_message, error_pos, multiple_errors = check_and_correct_hamming_code(hamming_code)

    if error_pos == 0:
        print("No errors detected.")
    elif not multiple_errors:
        print("Errors detected and corrected.")
    else:
        print("Multiple errors detected. Message discarded.")

    if not multiple_errors:
        print("Original message: ", ''.join(map(str, original_message)))

if __name__ == "__main__":
    main()
