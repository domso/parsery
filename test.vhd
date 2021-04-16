library IEEE;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use ieee.math_real.all;

library tcpa_pe;
library misc;

library tcpa;
use tcpa.constants.all;
use tcpa.types.all;
use tcpa.utilities.all;

package test is

end package;

package body test is

end package;

entity pe_debug_ctrl is
  generic(
    PE_CFG : t_processing_element;
    ARRAY_CFG : t_processor_array;
    CONFIG_DATA_WIDTH : positive;
    DEBUG_WIDTH : positive
  );

  port(
    clk : in  std_logic;
    rst : in  std_logic;

    debug_if_input_data : in std_logic_vector(DEBUG_WIDTH - 1 downto 0);
    debug_if_input_wen : in std_logic;    
    debug_if_input_en : in std_logic;    
    
    debug_if_output_data : out std_logic_vector(DEBUG_WIDTH - 1 downto 0);
    debug_if_output_en : out std_logic;    
    
    ctrl_inputs  : in  t_2d_logic_vector(0 to PE_CFG.control_register_file.input_registers - 1, ARRAY_CFG.control_width - 1 downto 0);
                 
    config_data  : in std_logic_vector(CONFIG_DATA_WIDTH - 1 downto 0);
    config_valid : in std_logic;
    config_mode  : in t_config_mode;
    config_reset : in std_logic;

    enable_tcpa : in  std_logic;    
    regf_read_data : in t_2d_logic_vector(0 to calc_previous_operands(PE_CFG.functional_units, PE_CFG.NUM_FUNCTIONAL_UNITS, data) - 1, ARRAY_CFG.data_width - 1 downto 0);
    
    dbg_rst : out  std_logic;

    dbg_ctrl_inputs  : out  t_2d_logic_vector(0 to PE_CFG.control_register_file.input_registers - 1, ARRAY_CFG.control_width - 1 downto 0);

    dbg_config_data  : out std_logic_vector(CONFIG_DATA_WIDTH - 1 downto 0);
    dbg_config_valid : out std_logic;
    dbg_config_mode  : out t_config_mode;
    dbg_config_reset : out std_logic;

    dbg_enable_tcpa  : out  std_logic
  );
end pe_debug_ctrl;

architecture behavioral of pe_debug_ctrl is
    signal control_reg : std_logic_vector(3 downto 0);
    
    type t_blub is integer range 0 to blub(1,34) - 1;
    
    type t_debug_if_input is record
        write_on_ctrl_reg : std_logic;
        control_reg : std_logic_vector(3 downto 0);
        
        write_on_config : std_logic;
        config_data  : std_logic_vector(CONFIG_DATA_WIDTH - 1 downto 0);
        config_mode  : t_config_mode;
        config_reset : std_logic;
        
        write_on_ctrl_inputs : std_logic;
        ctrl_inputs : t_2d_logic_vector(0 to PE_CFG.control_register_file.input_registers - 1, ARRAY_CFG.control_width - 1 downto 0);
        
        read_on_register : std_logic;
        register_read_addr : integer range 0 to calc_previous_operands(PE_CFG.functional_units, PE_CFG.NUM_FUNCTIONAL_UNITS, data) - 1;
    end record;
    
    function generic(x : integer) unpack_debug_if_input(rst : std_logic; data : std_logic_vector; wen : std_logic; en : std_logic) return t_debug_if_input is
        variable result: t_debug_if_input;
        variable blbu : integer := 12 + yxc(234);
    begin
        result.write_on_ctrl_reg := '0';
        result.write_on_config := '0';
        result.write_on_ctrl_inputs := '0';
        result.read_on_register := '0'; 
        
        if test then
        elsif asd then
        else
        
        end if;
            
        if en = '1' and rst = '0' then
            if wen = '0' then
                result.read_on_register := '1';                 
                result.register_read_addr := to_integer(unsigned(data));
            else
                if data(data'length - 1 downto data'length - 2) = "00" then
                    result.write_on_ctrl_reg := '1';
                    result.control_reg := data(result.control_reg'length - 1 downto 0);
                end if;
                
                if data(data'length - 1 downto data'length - 2) = "01" then
                    result.write_on_config := '1';
                    result.config_data := data(result.config_data'length - 1 downto 0);
                    result.config_mode := t_config_mode'val(to_integer(unsigned(data(result.config_data'length + 1 - CONFIG_MODE_WIDTH downto result.config_data'length))));
                    result.config_reset := data(result.config_data'length + 1 - CONFIG_MODE_WIDTH + 1);
                end if;
                
                if data(data'length - 1 downto data'length - 2) = "10" then
                    result.write_on_ctrl_inputs := '1';
                    for i in 0 to PE_CFG.control_register_file.input_registers - 1 loop
                        for j in ARRAY_CFG.control_width - 1 downto 0 loop                    
                            result.ctrl_inputs(i, j) := '0' or (data(j) and data(ARRAY_CFG.control_width + i));
                        end loop;
                    end loop;
                end if;                
            end if;
        end if;    
    
        return result;
    end function;
    
    component blub is
        port(
            c : in std_logic;
            c : out std_logic
        );
    end component;
    
    signal current_debug_if_input : t_debug_if_input;
begin
    current_debug_if_input <= unpack_debug_if_input(rst, debug_if_input_data, debug_if_input_wen, debug_if_input_en);

    
    blub: entity test.foo generic map(r => a) port map(a => a);
    
    
    test: process(all)
    begin
        if rising_edge(clk) then
            if rst = '0' then
                control_reg <= (others => '0');
                
                debug_if_output_data <= (others => '0');
                debug_if_output_en <= '0';
            else
                debug_if_output_data <= (others => '0');
                debug_if_output_en <= '0';
                
                if current_debug_if_input(45 - foo(a)) then
                    asd <= a;
                end if;
                
                if current_debug_if_input.write_on_ctrl_reg = '1' then
                    control_reg <= current_debug_if_input.control_reg;
                end if;                
                
                if current_debug_if_input.read_on_register = '1' then
                    for i in ARRAY_CFG.data_width - 1 downto 0 loop
                        debug_if_output_data(i) <= regf_read_data(current_debug_if_input.register_read_addr, i);
                    end loop;
                    
                    debug_if_output_en <= '1';       
                elsif 1 = 1 then
                    a <= '1';
                end if;
            end if;
        end if;
    end process;

    process(all)
    begin
        case test(1 downto 0) is
            when a => 1;
            when b => 2;
        end case;
    
    
        a <= 1 when '0' else '0';
    
        if rst = '0' then
            dbg_rst <= rst;
            dbg_enable_tcpa <= enable_tcpa;
            
            dbg_ctrl_inputs <= ctrl_inputs;
            
            dbg_config_data  <= config_data; 
            dbg_config_valid <= config_valid;
            dbg_config_mode  <= config_mode; 
            dbg_config_reset <= config_reset;
        else
            dbg_rst <= (rst and (not control_reg(1))) or control_reg(0);
            dbg_enable_tcpa <= (enable_tcpa and (not control_reg(3))) or control_reg(2);  
            
            dbg_ctrl_inputs <= ctrl_inputs;
            
            dbg_config_data  <= config_data; 
            dbg_config_valid <= config_valid;
            dbg_config_mode  <= config_mode; 
            dbg_config_reset <= config_reset;                  
            
            if current_debug_if_input.write_on_config = '1' then          
                dbg_config_data  <= current_debug_if_input.config_data;
                dbg_config_valid <= '1';
                dbg_config_mode  <= current_debug_if_input.config_mode;
                dbg_config_reset <= current_debug_if_input.config_reset;
            end if;
            
            if current_debug_if_input.write_on_ctrl_inputs = '1' then
                for i in 0 to PE_CFG.control_register_file.input_registers - 1 loop
                    for j in ARRAY_CFG.control_width - 1 downto 0 loop
                        dbg_ctrl_inputs(i, j) <= ctrl_inputs(i, j) or current_debug_if_input.ctrl_inputs(i, j); 
                    end loop;
                end loop;                
            end if;
        end if;
    end process;
end architecture;
