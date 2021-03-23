/**
 * Copyright (C) 2021 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <tee_internal_api.h>
#include <trace.h>

#include "fde_key_handler_ta_type.h"
#include "fde_key_handler_ta_handle.h"

// TA lock status
static int _ta_lock;

static TEE_Result lock_ta( uint32_t param_types,
                           TEE_Param params[TEE_NUM_PARAMS]);
static TEE_Result get_ta_lock( uint32_t param_types,
                               TEE_Param params[TEE_NUM_PARAMS]);

static TEE_Result print_debug( uint32_t paramTypes,
                              TEE_Param params[TEE_NUM_PARAMS]) {

   if (paramTypes != TEE_PARAM_TYPES(TEE_PARAM_TYPE_MEMREF_INPUT,
                                    TEE_PARAM_TYPE_NONE,
                                    TEE_PARAM_TYPE_NONE,
                                    TEE_PARAM_TYPE_NONE))
      return TEE_ERROR_BAD_PARAMETERS;

  DLOG("FDE_TA:REE_LOG: %s\n",(char *)params[0].memref.buffer);
  return TEE_SUCCESS;
}

TEE_Result TA_CreateEntryPoint(void) {
    DMSG("fde_key_handler: TA_CreateEntryPoint\n");
    _ta_lock = TA_UNLOCKED;
    return TEE_SUCCESS;
}

TEE_Result TA_OpenSessionEntryPoint( uint32_t paramTypes,
                               TEE_Param __maybe_unused params[TEE_NUM_PARAMS],
                               void __maybe_unused **sessionContext) {

    DMSG("fde_key_handler: TA_OpenSessionEntryPoint\n");
    UNUSED(paramTypes);
    UNUSED(params);
    UNUSED(sessionContext);
    return TEE_SUCCESS;
}

void TA_CloseSessionEntryPoint( void __maybe_unused *session_context) {
    DMSG("fde_key_handler: TA_CloseSessionEntryPoint\n");
    UNUSED(session_context);
}

void TA_DestroyEntryPoint(void) {
    DMSG("fde_key_handler: TA_DestroyEntryPoint\n");
}

TEE_Result TA_InvokeCommandEntryPoint( void __maybe_unused *session_context,
                                       uint32_t cmd_id,
                                       uint32_t paramTypes,
                                       TEE_Param params[TEE_NUM_PARAMS]) {

    DLOG("FDE cmd_id = %#"PRIx32"\n", cmd_id);
    switch (cmd_id) {
        case TA_CMD_KEY_ENCRYPT:
            return key_crypto(TEE_MODE_ENCRYPT, paramTypes, params);
        case TA_CMD_KEY_DECRYPT:
            // make sure crypto opperations are not locked
            if ( _ta_lock == TA_LOCKED) {
                DMSG("TA is locked for further decrypt oprerations!!");
                return TEE_ERROR_ACCESS_DENIED;
            }
            return key_crypto(TEE_MODE_DECRYPT, paramTypes, params);
        case TA_CMD_LOCK:
            return lock_ta(paramTypes, params);
        case TA_CMD_GET_LOCK:
      	  	return get_ta_lock(paramTypes, params);
        case TA_CMD_GEN_RANDOM:
            return generate_random(paramTypes, params);
        case TA_CMD_DEBUG:
            return print_debug(paramTypes, params);
        default:
            EMSG("Command ID %#"PRIx32" is not supported", cmd_id);
            return TEE_ERROR_NOT_SUPPORTED;
    }
}

static TEE_Result lock_ta( uint32_t paramTypes,
                           TEE_Param params[TEE_NUM_PARAMS]) {

    UNUSED(params);
    if (paramTypes != TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
                                    TEE_PARAM_TYPE_NONE,
                                    TEE_PARAM_TYPE_NONE,
                                    TEE_PARAM_TYPE_NONE))
        return TEE_ERROR_BAD_PARAMETERS;

    DMSG("Locking TA for further use\n");
    _ta_lock = TA_LOCKED;
  	return TEE_SUCCESS;
}

static TEE_Result get_ta_lock( uint32_t paramTypes,
                               TEE_Param params[TEE_NUM_PARAMS]) {

    if (paramTypes != TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_OUTPUT,
                                    TEE_PARAM_TYPE_NONE,
                                    TEE_PARAM_TYPE_NONE,
                                    TEE_PARAM_TYPE_NONE))
        return TEE_ERROR_BAD_PARAMETERS;

    params[0].value.a = _ta_lock;
  	return TEE_SUCCESS;
}
