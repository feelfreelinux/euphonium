import { createAction, createReducer } from "@reduxjs/toolkit";

export enum WebsocketStatus {
  CONNECTING = "CONNECTING",
  WAITING_FOR_RECONNECT = "WAITING_FOR_RECONNECT",
  CONNECTED = "CONNECTED",
}

export interface WebsocketState {
  status: WebsocketStatus;
  ping: number | null;
}

const initialState: WebsocketState = {
  status: WebsocketStatus.CONNECTING,
  ping: null,
};

export const statusChanged = createAction<WebsocketStatus>(
  "websocket/statusChanged"
);

export const pingReceived = createAction<number>("websocket/pingReceived");

export const websocketReducer = createReducer(initialState, (builder) => {
  builder
    .addCase(statusChanged, (state, action) => {
      state.status = action.payload;
    })
    .addCase(pingReceived, (state, action) => {
      state.ping = action.payload;
    });
});
